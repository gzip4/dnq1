// Multiprocessor support
// Search memory for MP description structures.
// http://developer.intel.com/design/pentium/datashts/24201606.pdf

#include <dnq/dnq.h>
#include <config.h>
#include "subc.h"

#if defined(CONFIG_SMP)

// See MultiProcessor Specification Version 1.[14]


struct mp {		// floating pointer
	uint8 signature[4];	// "_MP_"
	void *physaddr;		// phys addr of MP config table
	uint8 length;		// 1
	uint8 specrev;		// [14]
	uint8 checksum;		// all bytes must add up to 0
	uint8 type;		// MP system config type
	uint8 imcrp;
	uint8 reserved[3];
};

struct mpconf {		// configuration table header
	uint8 signature[4];	// "PCMP"
	uint16 length;		// total table length
	uint8 version;		// [14]
	uint8 checksum;		// all bytes must add up to 0
	uint8 product[20];	// product id
	uint32 *oemtable;	// OEM table pointer
	uint16 oemlength;	// OEM table length
	uint16 entry;		// entry count
	uint32 *lapicaddr;	// address of local APIC
	uint16 xlength;		// extended table length
	uint8 xchecksum;	// extended table checksum
	uint8 reserved;
};

struct mpproc {		// processor table entry
	uint8 type;		// entry type (0)
	uint8 apicid;		// local APIC id
	uint8 version;		// local APIC verison
	uint8 flags;		// CPU flags
    #define MPBOOT 0x02		// This proc is the bootstrap processor.
	uint8 signature[4];	// CPU signature
	uint32 feature;		// feature flags from CPUID instruction
	uint8 reserved[8];
};

struct mpioapic {	// I/O APIC table entry
	uint8 type;		// entry type (2)
	uint8 apicno;		// I/O APIC id
	uint8 version;		// I/O APIC version
	uint8 flags;		// I/O APIC flags
	uint32 *addr;		// I/O APIC address
};

// Table entry types
#define MPPROC    0x00		// One per processor
#define MPBUS     0x01		// One per bus
#define MPIOAPIC  0x02		// One per I/O APIC
#define MPIOINTR  0x03		// One per bus interrupt source
#define MPLINTR   0x04		// One per system interrupt source




static inline void outb(uint16 port, uint8 val)
{
	__asm__ __volatile__ (
		"outb %0, %1"
		: /* no output */
		: "a"(val), "Nd"(port)
	);
}

static inline uint8 inb(uint16 port)
{
	uint8 ret;
	__asm__ __volatile__ (
		"inb %1, %0"
		: "=a"(ret)
		: "Nd"(port)
	);
	return ret;
}


static int memcmp(const void *v1, const void *v2, uint32 n)
{
	const uint8 *s1, *s2;
	s1 = (const uint8 *) v1;
	s2 = (const uint8 *) v2;
	while (n-- > 0) {
		if (*s1 != *s2)
			return *s1 - *s2;
		s1++, s2++;
	}
	return 0;
}



struct cpu
{
	uint8 id;
};

extern volatile uint32 *lapic;
struct cpu cpus[NCPU];
static struct cpu *bcpu;
int ismp;
int ncpu;
uint8 ioapicid;

int mpbcpu(void)
{
	return bcpu-cpus;
}

static uint8 sum(uint8 *addr, int len)
{
	int i, sum;
	sum = 0;
	for (i=0; i<len; i++)
		sum += addr[i];
	return (uint8) sum;
}

// Look for an MP structure in the len bytes at addr.
static struct mp* mpsearch1(uint32 a, int len)
{
	uint8 *e, *p, *addr;
	addr = (uint8 *) a;
	e = addr+len;
	for (p = addr; p < e; p += sizeof(struct mp))
		if (memcmp(p, "_MP_", 4) == 0 && sum(p, sizeof(struct mp)) == 0)
			return (struct mp*)p;
	return 0;
}

// Search for the MP Floating Pointer Structure, which according to the
// spec is in one of the following three locations:
// 1) in the first KB of the EBDA;
// 2) in the last KB of system base memory;
// 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
static struct mp* mpsearch(void)
{
	uint8 *bda;
	uint32 p;
	struct mp *mp;

	bda = (uint8 *) 0x400;
	if ((p = (uint32)((bda[0x0F]<<8)| bda[0x0E]) << 4)) {
		if ((mp = mpsearch1(p, 1024)))
			return mp;
	} else {
		p = (uint32)((bda[0x14]<<8)|bda[0x13])*1024;
		if ((mp = mpsearch1(p-1024, 1024)))
			return mp;
	}
	return mpsearch1(0xF0000, 0x10000);
}

// Search for an MP configuration table.  For now,
// don't accept the default configurations (physaddr == 0).
// Check for correct signature, calculate the checksum and,
// if correct, check the version.
// To do: check extended table checksum.
static struct mpconf* mpconfig(struct mp **pmp)
{
	struct mpconf *conf;
	struct mp *mp;

	if ((mp = mpsearch()) == 0 || mp->physaddr == 0)
		return 0;

	conf = (struct mpconf*) ((uint32) mp->physaddr);
	if (memcmp(conf, "PCMP", 4) != 0)
		return 0;
	if (conf->version != 1 && conf->version != 4)
		return 0;
	if (sum((uint8*)conf, conf->length) != 0)
		return 0;
	*pmp = mp;
	return conf;
}

void mpinit()
{
	uint8 *p, *e;
	struct mp *mp;
	struct mpconf *conf;
	struct mpproc *proc;
	struct mpioapic *ioapic;

	bcpu = &cpus[0];
	if ((conf = mpconfig(&mp)) == 0)
		return;
	ismp = 1;
	lapic = (uint32*)conf->lapicaddr;
	for (p=(uint8*)(conf+1), e=(uint8*)conf+conf->length; p<e; )
	{
		switch (*p) {
		case MPPROC:
			proc = (struct mpproc*)p;
			if (ncpu != proc->apicid) {
				printf("mpinit: ncpu=%d apicid=%d\n",
					ncpu, proc->apicid);
				ismp = 0;
			}
			if (proc->flags & MPBOOT)
				bcpu = &cpus[ncpu];
			cpus[ncpu].id = (uint8) ncpu;
			ncpu++;
			p += sizeof(struct mpproc);
			continue;
		case MPIOAPIC:
			ioapic = (struct mpioapic*)p;
			ioapicid = ioapic->apicno;
			p += sizeof(struct mpioapic);
			continue;
		case MPBUS:
		case MPIOINTR:
		case MPLINTR:
			p += 8;
			continue;
		default:
			printf("mpinit: unknown config type %x\n", *p);
			ismp = 0;
		}
	}

	if (!ismp) {
		// Didn't like what we found; fall back to no MP.
		ncpu = 1;
		lapic = 0;
		ioapicid = 0;
		return;
	}

	if (mp->imcrp) {
		// Bochs doesn't support IMCR, so this doesn't run on Bochs.
		// But it would on real hardware.
		outb(0x22, 0x70);   // Select IMCR
		outb(0x23, inb(0x23) | 1);  // Mask external interrupts.
	}
}

#endif // defined(CONFIG_SMP)

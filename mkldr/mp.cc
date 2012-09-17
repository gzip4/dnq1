// Multiprocessor support
// Search memory for MP description structures.
// http://developer.intel.com/design/pentium/datashts/24201606.pdf

#include <config.h>
#include "subc.h"

#if defined(CONFIG_SMP)

// See MultiProcessor Specification Version 1.[14]

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;


struct mp {		// floating pointer
	uchar signature[4];	// "_MP_"
	void *physaddr;		// phys addr of MP config table
	uchar length;		// 1
	uchar specrev;		// [14]
	uchar checksum;		// all bytes must add up to 0
	uchar type;		// MP system config type
	uchar imcrp;
	uchar reserved[3];
};

struct mpconf {		// configuration table header
	uchar signature[4];	// "PCMP"
	ushort length;		// total table length
	uchar version;		// [14]
	uchar checksum;		// all bytes must add up to 0
	uchar product[20];	// product id
	uint *oemtable;		// OEM table pointer
	ushort oemlength;	// OEM table length
	ushort entry;		// entry count
	uint *lapicaddr;	// address of local APIC
	ushort xlength;		// extended table length
	uchar xchecksum;	// extended table checksum
	uchar reserved;
};

struct mpproc {		// processor table entry
	uchar type;		// entry type (0)
	uchar apicid;		// local APIC id
	uchar version;		// local APIC verison
	uchar flags;		// CPU flags
    #define MPBOOT 0x02		// This proc is the bootstrap processor.
	uchar signature[4];	// CPU signature
	uint feature;		// feature flags from CPUID instruction
	uchar reserved[8];
};

struct mpioapic {	// I/O APIC table entry
	uchar type;		// entry type (2)
	uchar apicno;		// I/O APIC id
	uchar version;		// I/O APIC version
	uchar flags;		// I/O APIC flags
	uint *addr;		// I/O APIC address
};

// Table entry types
#define MPPROC    0x00		// One per processor
#define MPBUS     0x01		// One per bus
#define MPIOAPIC  0x02		// One per I/O APIC
#define MPIOINTR  0x03		// One per bus interrupt source
#define MPLINTR   0x04		// One per system interrupt source




static inline void outb(ushort port, uchar val)
{
	__asm__ __volatile__ (
		"outb %0, %1"
		: /* no output */
		: "a"(val), "Nd"(port)
	);
}

static inline uchar inb(ushort port)
{
	uchar ret;
	__asm__ __volatile__ (
		"inb %1, %0"
		: "=a"(ret)
		: "Nd"(port)
	);
	return ret;
}


static int memcmp(const void *v1, const void *v2, uint n)
{
	const uchar *s1, *s2;
	s1 = (const uchar *) v1;
	s2 = (const uchar *) v2;
	while (n-- > 0) {
		if (*s1 != *s2)
			return *s1 - *s2;
		s1++, s2++;
	}
	return 0;
}



struct cpu
{
	uchar id;
};

extern volatile uint *lapic;
struct cpu cpus[NCPU];
static struct cpu *bcpu;
int ismp;
int ncpu;
uchar ioapicid;

int mpbcpu(void)
{
	return bcpu-cpus;
}

static uchar sum(uchar *addr, int len)
{
	int i, sum;
	sum = 0;
	for (i=0; i<len; i++)
		sum += addr[i];
	return (uchar) sum;
}

// Look for an MP structure in the len bytes at addr.
static struct mp* mpsearch1(uint a, int len)
{
	uchar *e, *p, *addr;
	addr = (uchar *) a;
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
	uchar *bda;
	uint p;
	struct mp *mp;

	bda = (uchar *) 0x400;
	if ((p = (uint)((bda[0x0F]<<8)| bda[0x0E]) << 4)) {
		if ((mp = mpsearch1(p, 1024)))
			return mp;
	} else {
		p = (uint)((bda[0x14]<<8)|bda[0x13])*1024;
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

	conf = (struct mpconf*) ((uint) mp->physaddr);
	if (memcmp(conf, "PCMP", 4) != 0)
		return 0;
	if (conf->version != 1 && conf->version != 4)
		return 0;
	if (sum((uchar*)conf, conf->length) != 0)
		return 0;
	*pmp = mp;
	return conf;
}

void mpinit()
{
	uchar *p, *e;
	struct mp *mp;
	struct mpconf *conf;
	struct mpproc *proc;
	struct mpioapic *ioapic;

	bcpu = &cpus[0];
	if ((conf = mpconfig(&mp)) == 0)
		return;
	ismp = 1;
	lapic = (uint*)conf->lapicaddr;
	for (p=(uchar*)(conf+1), e=(uchar*)conf+conf->length; p<e; )
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
			cpus[ncpu].id = (uchar) ncpu;
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

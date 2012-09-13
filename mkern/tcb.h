#ifndef MKERN_TCB_H
#define MKERN_TCB_H

#include <dnq/dnq.h>

// FIXME! move this define to config.h
#define NTCB	1024

class tcb_t
{
public:


private:
	friend void init_tcb();
	static tcb_t *alloc();
	static void free(tcb_t *);


private:
	enum { FREE, INACTIVE, READY, RUNNING,
	       SEND, RECV, RECV2, PAGEF, EXPT,
	} state;

	// TCB QUEUES
	tcb_t *next;
	tcb_t *qrecv;
	tcb_t *qsend;

private:
	tcb_t();
	tcb_t(const tcb_t&);
	tcb_t& operator=(const tcb_t&);
};

extern void init_tcb();


#endif /* MKERN_TCB_H */

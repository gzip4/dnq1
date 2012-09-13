#include "tcb.h"

static char _tcbs[NTCB * sizeof(tcb_t)];
static tcb_t *const tcb_begin = reinterpret_cast<tcb_t*>(_tcbs);
static tcb_t *const tcb_end = tcb_begin + NTCB;
static tcb_t *tcb_free = tcb_begin;


void init_tcb()
{
	tcb_t *tcb;
	for (tcb = tcb_begin; tcb < tcb_end; ++tcb) {
		tcb->state = tcb_t::FREE;
		tcb->next = tcb + 1;
	}
	tcb->next = 0;
}


tcb_t *tcb_t::alloc()
{
	tcb_t *tcb = tcb_free;
	if (tcb) {
		tcb_free = tcb_free->next;
		tcb->next = 0;
		tcb->state = INACTIVE;
	}
	return tcb;
}

void tcb_t::free(tcb_t *tcb /* != NULL */)
{
	tcb->state = FREE;
	tcb->next = tcb_free;
	tcb_free = tcb;
}

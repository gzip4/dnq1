#include "spinlock.h"

extern "C" void trap(void *);



static void trap_handler(void *)
{
	__asm__ __volatile__ ("nop");
}


spinlock_t GIANT_LOCK = 0;

void trap(void *trapfrm)
{
	spinlock_accuire(GIANT_LOCK);

	// CRITICAL SECTION!
	// now we're allowed to change any global
	// kernel state (but do it quickly)

	// current == next (&& maybe == idle)

	trap_handler(trapfrm);

	// next may be changed
	// possible thread transitions:
	// current	next
	// -----------------------------
	// idle		idle
	// idle		some thread
	// current	idle
	// current	current
	// current	some thread

	// XXX: save current thread context
	// XXX: load new thread context
	// XXX: load new thread vspace
	// XXX: load new thread ldtr
	// XXX: load new thread tss
	// XXX: setup new thread iopl



	spinlock_release(GIANT_LOCK);

	// XXX: set current = next
}

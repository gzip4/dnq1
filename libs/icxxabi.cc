// see: http://wiki.osdev.org/C++

#include "icxxabi.h"

EXTERN_C_BEGIN

void __cxa_pure_virtual() {
	// Do Nothing
}

atexitFuncEntry_t __atexitFuncs[ATEXIT_FUNC_MAX];
uarch_t __atexitFuncCount = 0;

void *__dso_handle = 0;

int __cxa_atexit(void (*f)(void *), void *objptr, void *dso){
	if(__atexitFuncCount >= ATEXIT_FUNC_MAX){
		return -1;
	}
	__atexitFuncs[__atexitFuncCount].destructorFunc = f;
	__atexitFuncs[__atexitFuncCount].objPtr = objptr;
	__atexitFuncs[__atexitFuncCount].dsoHandle = dso;
	__atexitFuncCount++;
	return 0;
}

void __cxa_finalize(void *f){
	signed i = __atexitFuncCount;
	if(!f){
		while(i--){
			if(__atexitFuncs[i].destructorFunc){
				(*__atexitFuncs[i].destructorFunc)(__atexitFuncs[i].objPtr);
			}
		}
		return;
	}

	for(; i >= 0; i--){
		if(__atexitFuncs[i].destructorFunc == f){
			(*__atexitFuncs[i].destructorFunc)(__atexitFuncs[i].objPtr);
			__atexitFuncs[i].destructorFunc = 0;
		}
	}
}


namespace __cxxabiv1
{
	/* guard variables */
	
	/* The ABI requires a 64-bit type.  */
	__extension__ typedef int __guard __attribute__((mode(__DI__)));
	
	int __cxa_guard_acquire (__guard *);
	void __cxa_guard_release (__guard *);
	void __cxa_guard_abort (__guard *);

	int __cxa_guard_acquire (__guard *g) 
	{
		return !*(char *)(g);
	}

	void __cxa_guard_release (__guard *g)
	{
		*(char *)g = 1;
	}

	void __cxa_guard_abort (__guard *)
	{

	}
}


EXTERN_C_END

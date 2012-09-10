#ifndef ICXXABI_H_
#define ICXXABI_H_

// see: http://wiki.osdev.org/C++

#define ATEXIT_FUNC_MAX 128

#ifdef __cplusplus
#define EXTERN_C_BEGIN	extern "C" {
#define EXTERN_C_END	}
#else
#define EXTERN_C_BEGIN	extern "C" {
#define EXTERN_C_END	}
#endif

EXTERN_C_BEGIN

typedef unsigned uarch_t;

struct atexitFuncEntry_t {
	void (*destructorFunc) (void *);
	void *objPtr;
	void *dsoHandle;
};

extern void *__dso_handle;

int __cxa_atexit(void (*f)(void *), void *objptr, void *dso);
void __cxa_finalize(void *f);

void __cxa_pure_virtual();


EXTERN_C_END

#endif //ICXXABI_H_

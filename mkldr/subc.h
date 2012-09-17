#ifndef MKLDR_SUBC_H
#define MKLDR_SUBC_H

#include <stdarg.h>

extern "C" int printf(const char *, ...);
extern "C" void putc(int);
extern "C" int vsnprintf (char *, unsigned int, const char *, va_list);

extern "C" void *memcpy(void *dest, const void *src, unsigned n);


#endif /* MKLDR_SUBC_H */

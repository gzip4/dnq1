#ifndef SUBC_TYPES_H
#define SUBC_TYPES_H

#ifdef __GNUC__
typedef __SIZE_TYPE__		size_t;
typedef __PTRDIFF_TYPE__	ptrdiff_t;
#else
typedef unsigned long		size_t;
typedef long			ptrdiff_t;
#endif

#endif /* SUBC_TYPES_H */

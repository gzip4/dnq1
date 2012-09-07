#ifndef DNQ_TYPES_H
#define DNQ_TYPES_H

#ifdef __i386__
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

typedef signed char		sint8;
typedef signed short		sint16;
typedef signed int		sint32;
typedef signed long long	sint64;

typedef uint32			word_t;

#else
#error i386+ arch only
#endif /* __i386__ */


#endif /* DNQ_TYPES_H */

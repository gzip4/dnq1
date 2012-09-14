#ifndef DNQ_TID_H
#define DNQ_TID_H

#include <dnq/types.h>

#ifdef __i386__
#define VERSION_SHIFT	14
#define VERSION_MASK	((1<<VERSION_SHIFT)-1)
#else
#error unsupported arch
#endif

struct tid_t
{
	mword_t raw;

	int no()  const { return raw >> VERSION_SHIFT; }
	int ver() const { return raw &  VERSION_MASK; }

	static const tid_t nil() {tid_t x = { 0 };return x;}
	static const tid_t any() {tid_t x = {-1U};return x;}

	template <typename T1, typename T2>
	static tid_t get(T1 no, T2 ver = 0)
	{
		tid_t tid = { (mword_t)(no << VERSION_SHIFT)
			| (ver & VERSION_MASK) };
		return tid;
	}

	static tid_t vnext(tid_t x)
	{
		bool vmax = (x.ver() == VERSION_MASK);
		return get(x.no(), vmax ? 0 : x.ver() + 1);
	}
};

#undef VERSION_SHIFT
#undef VERSION_MASK

#endif /* DNQ_TID_H */

#ifndef _HENCOMMONINCLUDE_H_
#define _HENCOMMONINCLUDE_H_

// private header used by engine cpp files, DONT INCLUDE IN HEADERS

template <typename T>
constexpr T Clamp(T x, T a, T b)
{
	return x < a ? a : (x > b ? b : x);
}

#endif // !_HENCOMMONINCLUDE_H_
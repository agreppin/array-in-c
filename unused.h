#ifndef UNUSED
#ifdef __GNUC__
#define UNUSED(x) x __attribute__((unused))
#elif defined(_MSC_VER)
#define UNUSED(x) __pragma(warning(suppress : 4100)) x
#elif defined(__LCLINT__)
#define UNUSED(x) /*@unused@*/ x
#else
#define UNUSED(x) x
#endif
#endif

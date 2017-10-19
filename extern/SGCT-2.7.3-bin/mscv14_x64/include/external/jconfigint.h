#define VERSION "1.4.1"
#define BUILD "20150923"
#define PACKAGE_NAME "sgct"

#ifndef INLINE
#if defined(__GNUC__)
#define INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define INLINE __forceinline
#else
#define INLINE
#endif
#endif

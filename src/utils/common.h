#ifndef __utils_h__
#define __utils_h__

#include <vector>
#include <list>
#include <set>
#include <string>
#include <cmath>
#include <chrono>
#include <ctime>
#include <unistd.h>

#include <cstdio>

#ifndef ERR
#define ERR(...)                                         \
    do {                                                 \
        fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                    \
        fflush(stderr);                                  \
    } while (0)
#endif

#ifndef DBG
#define DBG(...)                      \
    do {                              \
        fprintf(stdout, __VA_ARGS__); \
        fflush(stdout);               \
    } while (0)
#endif


#endif

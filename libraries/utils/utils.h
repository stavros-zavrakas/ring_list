#include <stdlib.h>
#include <sys/time.h>
#include <inttypes.h>

#ifndef _MAX
#define _MAX(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef _MIN
#define _MIN(a,b) (((a)<(b))?(a):(b))
#endif

uint64_t getTimeMillisecs();

uint64_t getTimeUsecs();
#include "utils.h"

uint64_t getTimeMillisecs() {
	struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(((uint64_t)tv.tv_sec * 1000) + ((uint64_t)tv.tv_usec / 1000.0));
}

uint64_t getTimeUsecs() {
	struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(((uint64_t)tv.tv_sec * 1000000) + ((uint64_t)tv.tv_usec));
}
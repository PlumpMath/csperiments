#include <stdlib.h>
#include <stdio.h>

#include <mach/mach_time.h>

static mach_timebase_info_data_t info;
static void __attribute__((constructor)) init_info() {
    mach_timebase_info(&info);
}

#define NANOS_PER_SECF 1000000000.0

double monotonic_seconds() {
    uint64_t time = mach_absolute_time();
    double dtime = (double) time;
    dtime *= (double) info.numer;
    dtime /= (double) info.denom;
    return dtime / NANOS_PER_SECF;
}

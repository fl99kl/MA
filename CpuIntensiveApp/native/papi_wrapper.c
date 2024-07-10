#include <papi.h>

int init_papi() {
    return PAPI_library_init(PAPI_VER_CURRENT);
}

int start_counters() {
    return PAPI_start_counters();
}

int stop_counters(long long *values, int num_events) {
    return PAPI_stop_counters(values, num_events);
}

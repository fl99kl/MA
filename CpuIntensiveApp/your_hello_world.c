#include <stdio.h>
#include <papi.h>

#define NUM_EVENTS 1
#define EVENT PAPI_TOT_CYC  // Total cycles

int hello_papi() {
    int retval;
    long long values[NUM_EVENTS];

    // Initialize PAPI library
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT && retval > 0) {
        fprintf(stderr, "PAPI library version mismatch!\n");
        return 1;
    }
    if (retval < 0) {
        fprintf(stderr, "Failed to initialize PAPI library!\n");
        return 1;
    }

    // Define events to measure, e.g., PAPI_TOT_CYC (total cycles)
    int events[2] = {PAPI_TOT_CYC, PAPI_TOT_INS};
    long long values[2];
    int ret;

    // Start counting
    if ((ret = PAPI_start_counters(events, 2)) != PAPI_OK) {
        fprintf(stderr, "PAPI failed to start counters: %s\n", PAPI_strerror(ret));
        return 1;
    }

    // Your application code here
    printf("Hello, World!\n");


    if ((ret = PAPI_stop_counters(values, 2)) != PAPI_OK) {
        fprintf(stderr, "PAPI failed to stop counters: %s\n", PAPI_strerror(ret));
        return 1;
    }

    // Print the counter values
    printf("Total cycles: %lld\n", values[0]);
    printf("Total instructions: %lld\n", values[1]);
    // Finalize PAPI library
    PAPI_shutdown();

    return 0;
}


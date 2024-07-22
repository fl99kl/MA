#include <stdio.h>
#include <papi.h>

#define NUM_EVENTS 1
#define EVENT PAPI_TOT_CYC  // Total cycles

int main() {
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

    // Your application code here
    printf("Hello, World!\n");

    // Finalize PAPI library
    PAPI_shutdown();

    return 0;
}


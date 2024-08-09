#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_RAPL_EVENTS 64

typedef struct {
    int EventSet;
    long long before_time;
    int num_events;
    char event_names[MAX_RAPL_EVENTS][PAPI_MAX_STR_LEN];
    char units[MAX_RAPL_EVENTS][PAPI_MIN_STR_LEN];
    int data_type[MAX_RAPL_EVENTS];
} RaplData;

void handle_error (int retval)
{
    printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
    exit(1);
}

RaplData* startRapl() {
    int retval, cid, rapl_cid = -1, numcmp;
    int code, r;
    RaplData* raplData = (RaplData*)malloc(sizeof(RaplData));
    const PAPI_component_info_t *cmpinfo = NULL;
    PAPI_event_info_t evinfo;

    raplData->EventSet = PAPI_NULL;
    raplData->num_events = 0;

    // Initialize the library
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) handle_error(retval);

    numcmp = PAPI_num_components();

    for (cid = 0; cid < numcmp; cid++) {
        if ((cmpinfo = PAPI_get_component_info(cid)) == NULL) {
            printf("PAPI_get_component_info failed\n");
            exit(1);
        }

        if (strstr(cmpinfo->name, "rapl")) {
            rapl_cid = cid;
            printf("Found rapl component at cid %d\n", rapl_cid);

            if (cmpinfo->disabled) {
                printf("RAPL component disabled\n");
                exit(1);
            }
            break;
        }
    }

    // Component not found
    if (cid == numcmp) {
        printf("No rapl component found\n");
        exit(1);
    }

    // Create an EventSet
    retval = PAPI_create_eventset(&(raplData->EventSet));
    if (retval != PAPI_OK) handle_error(retval);

    // Add all events
    code = PAPI_NATIVE_MASK;
    r = PAPI_enum_cmp_event(&code, PAPI_ENUM_FIRST, rapl_cid);

    while (r == PAPI_OK) {
        retval = PAPI_event_code_to_name(code, raplData->event_names[raplData->num_events]);
        if (retval != PAPI_OK) {
            printf("Error translating %#x\n", code);
            handle_error(retval);
        }

        retval = PAPI_get_event_info(code, &evinfo);
        if (retval != PAPI_OK) handle_error(retval);

        strncpy(raplData->units[raplData->num_events], evinfo.units, sizeof(raplData->units[0]));
        raplData->units[raplData->num_events][sizeof(raplData->units[0]) - 1] = '\0';
        raplData->data_type[raplData->num_events] = evinfo.data_type;

        retval = PAPI_add_event(raplData->EventSet, code);
        if (retval != PAPI_OK) break; // We've hit an event limit
        raplData->num_events++;

        r = PAPI_enum_cmp_event(&code, PAPI_ENUM_EVENTS, rapl_cid);
    }

    printf("\nStarting measurements...\n\n");

    // Start Counting
    raplData->before_time = PAPI_get_real_nsec();
    retval = PAPI_start(raplData->EventSet);
    if (retval != PAPI_OK) handle_error(retval);

    return raplData;
}

void readAndStopRapl(RaplData* raplData) {
    int retval, i;
    long long after_time;
    double elapsed_time;
    long long* values = calloc(raplData->num_events, sizeof(long long));
    if (values == NULL) handle_error(PAPI_ENOMEM);

    printf("Sleeping 5 seconds...\n");
    sleep(5);

    // Stop Counting
    after_time = PAPI_get_real_nsec();
    retval = PAPI_stop(raplData->EventSet, values);
    if (retval != PAPI_OK) handle_error(retval);

    elapsed_time = ((double)(after_time - raplData->before_time)) / 1.0e9;

    printf("\nStopping measurements, took %.3fs, gathering results...\n\n", elapsed_time);

    printf("Scaled energy measurements:\n");
    for (i = 0; i < raplData->num_events; i++) {
        if (strstr(raplData->units[i], "nJ")) {
            printf("%-40s%12.6f J\t(Average Power %.1fW)\n",
                   raplData->event_names[i],
                   (double)values[i] / 1.0e9,
                   ((double)values[i] / 1.0e9) / elapsed_time);
        }
    }

    printf("\nEnergy measurement counts:\n");
    for (i = 0; i < raplData->num_events; i++) {
        if (strstr(raplData->event_names[i], "ENERGY_CNT")) {
            printf("%-40s%12lld\t%#08llx\n", raplData->event_names[i], values[i], values[i]);
        }
    }

    printf("\nScaled Fixed values:\n");
    for (i = 0; i < raplData->num_events; i++) {
        if (!strstr(raplData->event_names[i], "ENERGY")) {
            if (raplData->data_type[i] == PAPI_DATATYPE_FP64) {
                union {
                    long long ll;
                    double fp;
                } result;
                result.ll = values[i];
                printf("%-40s%12.3f %s\n", raplData->event_names[i], result.fp, raplData->units[i]);
            }
        }
    }

    printf("\nFixed value counts:\n");
    for (i = 0; i < raplData->num_events; i++) {
        if (!strstr(raplData->event_names[i], "ENERGY")) {
            if (raplData->data_type[i] == PAPI_DATATYPE_UINT64) {
                printf("%-40s%12lld\t%#08llx\n", raplData->event_names[i], values[i], values[i]);
            }
        }
    }

    free(values);
    free(raplData);
}
	
int runRapl()
{
    int retval,cid,rapl_cid=-1,numcmp;
    int EventSet = PAPI_NULL;
    long long *values;
    int num_events=0;
    int code;
    char event_names[MAX_RAPL_EVENTS][PAPI_MAX_STR_LEN];
    char units[MAX_RAPL_EVENTS][PAPI_MIN_STR_LEN];
    int data_type[MAX_RAPL_EVENTS];
    int r,i;
    const PAPI_component_info_t *cmpinfo = NULL;
    PAPI_event_info_t evinfo;
    long long before_time,after_time;
    double elapsed_time;
		 
    /* Initialize the library */
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT)
        handle_error(retval);

    numcmp = PAPI_num_components();

    for(cid=0; cid<numcmp; cid++) {

        if ( (cmpinfo = PAPI_get_component_info(cid)) == NULL) {
            printf("PAPI_get_component_info failed\n");
            exit(1);
        }

        if (strstr(cmpinfo->name,"rapl")) {

            rapl_cid=cid;

            printf("Found rapl component at cid %d\n",rapl_cid);

            if (cmpinfo->disabled) {
                printf("RAPL component disabled");
                exit(1);
            }
            break;
        }
    }

    /* Component not found */
    if (cid==numcmp) {
        printf("No rapl component found\n");
        exit(1);
    }
	
    /* Create an EventSet */
    retval = PAPI_create_eventset(&EventSet);
    if (retval != PAPI_OK)
        handle_error(retval);
	
    /* Add all events */

    code = PAPI_NATIVE_MASK;

    r = PAPI_enum_cmp_event( &code, PAPI_ENUM_FIRST, rapl_cid );

    while ( r == PAPI_OK ) {

        retval = PAPI_event_code_to_name( code, event_names[num_events] );
        if ( retval != PAPI_OK ) {
            printf("Error translating %#x\n",code);
            handle_error(retval);
        }

        retval = PAPI_get_event_info(code,&evinfo);
        if (retval != PAPI_OK) {
            handle_error(retval);
        }

        strncpy(units[num_events],evinfo.units,sizeof(units[0]));
        // buffer must be null terminated to safely use strstr operation on it below
        units[num_events][sizeof(units[0])-1] = '\0';

        data_type[num_events] = evinfo.data_type;

        retval = PAPI_add_event( EventSet, code );
        if (retval != PAPI_OK) {
            break; /* We've hit an event limit */
        }
        num_events++;

        r = PAPI_enum_cmp_event( &code, PAPI_ENUM_EVENTS, rapl_cid );
    }

    values=calloc(num_events,sizeof(long long));
    if (values==NULL) {
        handle_error(retval);
    }


    printf("\nStarting measurements...\n\n");

    /* Start Counting */
    before_time=PAPI_get_real_nsec();
    retval = PAPI_start( EventSet);
    if (retval != PAPI_OK) {
        handle_error(retval);
    }

    /* Run test */
    // TODO
    //run_test(TESTS_QUIET);
    printf("Sleeping 5 second...\n");
    /* Sleep */
    sleep(5);

    /* Stop Counting */
    after_time=PAPI_get_real_nsec();
    retval = PAPI_stop( EventSet, values);
    if (retval != PAPI_OK) {
        handle_error(retval);
    }

    elapsed_time=((double)(after_time-before_time))/1.0e9;

    printf("\nStopping measurements, took %.3fs, gathering results...\n\n",
       elapsed_time);

    printf("Scaled energy measurements:\n");

    for(i=0;i<num_events;i++) {
        if (strstr(units[i],"nJ")) {

            printf("%-40s%12.6f J\t(Average Power %.1fW)\n",
              event_names[i],
              (double)values[i]/1.0e9,
              ((double)values[i]/1.0e9)/elapsed_time);
        }
    }

    printf("\n");
    printf("Energy measurement counts:\n");

    for(i=0;i<num_events;i++) {
        if (strstr(event_names[i],"ENERGY_CNT")) {
            printf("%-40s%12lld\t%#08llx\n", event_names[i], values[i], values[i]);
        }
    }

    printf("\n");
    printf("Scaled Fixed values:\n");

    for(i=0;i<num_events;i++) {
        if (!strstr(event_names[i],"ENERGY")) {
            if (data_type[i] == PAPI_DATATYPE_FP64) {

                union {
                    long long ll;
                    double fp;
                } result;

                result.ll=values[i];
                printf("%-40s%12.3f %s\n", event_names[i], result.fp, units[i]);
            }
        }
    }

    printf("\n");
    printf("Fixed value counts:\n");

    for(i=0;i<num_events;i++) {
        if (!strstr(event_names[i],"ENERGY")) {
            if (data_type[i] == PAPI_DATATYPE_UINT64) {
                printf("%-40s%12lld\t%#08llx\n", event_names[i], values[i], values[i]);
            }
        }
    }
}

int main() {
    RaplData* raplData = startRapl();
    readAndStopRapl(raplData);
    return 0;
}

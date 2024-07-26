#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

void handle_error (int retval)
{
    printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
    exit(1);
}
	
int main()
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
            print("PAPI_get_component_info failed\n");
            exit(1);
        }

        if (strstr(cmpinfo->name,"rapl")) {

            rapl_cid=cid;

            printf("Found rapl component at cid %d\n",rapl_cid);

            if (cmpinfo->disabled) {
                print("RAPL component disabled");
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
        handle_error(retval)
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

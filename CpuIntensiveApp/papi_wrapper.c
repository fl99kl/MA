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

void handle_error(int retval, FILE *outputFile) {
    fprintf(outputFile, "PAPI error %d: %s\n", retval, PAPI_strerror(retval));
    fclose(outputFile);
    exit(1);
}

void outputStart(const char* output_file_path) {
    FILE *outputFile = fopen(output_file_path, "w");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }
    fprintf(outputFile, "Output start Debug\n");
    fclose(outputFile);
}

void outputEnd(const char* output_file_path) {
    FILE *outputFile = fopen(output_file_path, "w");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }
    fprintf(outputFile, "Output end Debug\n");
    fclose(outputFile);
}

RaplData* startRapl(const char* output_file_path) {
    FILE *outputFile = fopen(output_file_path, "w");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }

    int retval, cid, rapl_cid = -1, numcmp;
    int code, r;
    RaplData* raplData = (RaplData*)malloc(sizeof(RaplData));
    const PAPI_component_info_t *cmpinfo = NULL;
    PAPI_event_info_t evinfo;

    raplData->EventSet = PAPI_NULL;
    raplData->num_events = 0;

    // Initialize the library
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) handle_error(retval, outputFile);

    numcmp = PAPI_num_components();

    for (cid = 0; cid < numcmp; cid++) {
        if ((cmpinfo = PAPI_get_component_info(cid)) == NULL) {
            fprintf(outputFile, "PAPI_get_component_info failed\n");
            fclose(outputFile);
            exit(1);
        }

        if (strstr(cmpinfo->name, "rapl")) {
            rapl_cid = cid;
            fprintf(outputFile, "Found rapl component at cid %d\n", rapl_cid);

            if (cmpinfo->disabled) {
                fprintf(outputFile, "RAPL component disabled\n");
                fclose(outputFile);
                exit(1);
            }
            break;
        }
    }

    // Component not found
    if (cid == numcmp) {
        fprintf(outputFile, "No rapl component found\n");
        fclose(outputFile);
        exit(1);
    }

    // Create an EventSet
    retval = PAPI_create_eventset(&(raplData->EventSet));
    if (retval != PAPI_OK) handle_error(retval, outputFile);

    // Add all events
    code = PAPI_NATIVE_MASK;
    r = PAPI_enum_cmp_event(&code, PAPI_ENUM_FIRST, rapl_cid);

    while (r == PAPI_OK) {
        retval = PAPI_event_code_to_name(code, raplData->event_names[raplData->num_events]);
        if (retval != PAPI_OK) {
            fprintf(outputFile, "Error translating %#x\n", code);
            handle_error(retval, outputFile);
        }

        retval = PAPI_get_event_info(code, &evinfo);
        if (retval != PAPI_OK) handle_error(retval, outputFile);

        strncpy(raplData->units[raplData->num_events], evinfo.units, sizeof(raplData->units[0]));
        raplData->units[raplData->num_events][sizeof(raplData->units[0]) - 1] = '\0';
        raplData->data_type[raplData->num_events] = evinfo.data_type;

        retval = PAPI_add_event(raplData->EventSet, code);
        if (retval != PAPI_OK) break; // We've hit an event limit
        raplData->num_events++;

        r = PAPI_enum_cmp_event(&code, PAPI_ENUM_EVENTS, rapl_cid);
    }

    fprintf(outputFile, "\nStarting measurements...\n\n");

    // Start Counting
    raplData->before_time = PAPI_get_real_nsec();
    retval = PAPI_start(raplData->EventSet);
    if (retval != PAPI_OK) handle_error(retval, outputFile);

    fclose(outputFile);
    return raplData;
}

void readAndStopRapl(RaplData* raplData, const char* output_file_path) {
    FILE *outputFile = fopen(output_file_path, "a");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }

    int retval, i;
    long long after_time;
    double elapsed_time;
    long long* values = calloc(raplData->num_events, sizeof(long long));
    if (values == NULL) handle_error(PAPI_ENOMEM, outputFile);

    fprintf(outputFile, "Sleeping 5 seconds...\n");
    sleep(5);

    // Stop Counting
    after_time = PAPI_get_real_nsec();
    retval = PAPI_stop(raplData->EventSet, values);
    if (retval != PAPI_OK) handle_error(retval, outputFile);

    elapsed_time = ((double)(after_time - raplData->before_time)) / 1.0e9;

    fprintf(outputFile, "\nStopping measurements, took %.3fs, gathering results...\n\n", elapsed_time);

    fprintf(outputFile, "Scaled energy measurements:\n");
    for (i = 0; i < raplData->num_events; i++) {
        if (strstr(raplData->units[i], "nJ")) {
            fprintf(outputFile, "%-40s%12.6f J\t(Average Power %.1fW)\n",
                   raplData->event_names[i],
                   (double)values[i] / 1.0e9,
                   ((double)values[i] / 1.0e9) / elapsed_time);
        }
    }

    fprintf(outputFile, "\nEnergy measurement counts:\n");
    for (i = 0; i < raplData->num_events; i++) {
        if (strstr(raplData->event_names[i], "ENERGY_CNT")) {
            fprintf(outputFile, "%-40s%12lld\t%#08llx\n", raplData->event_names[i], values[i], values[i]);
        }
    }

    fprintf(outputFile, "\nScaled Fixed values:\n");
    for (i = 0; i < raplData->num_events; i++) {
        if (!strstr(raplData->event_names[i], "ENERGY")) {
            if (raplData->data_type[i] == PAPI_DATATYPE_FP64) {
                union {
                    long long ll;
                    double fp;
                } result;
                result.ll = values[i];
                fprintf(outputFile, "%-40s%12.3f %s\n", raplData->event_names[i], result.fp, raplData->units[i]);
            }
        }
    }

    fprintf(outputFile, "\nFixed value counts:\n");
    for (i = 0; i < raplData->num_events; i++) {
        if (!strstr(raplData->event_names[i], "ENERGY")) {
            if (raplData->data_type[i] == PAPI_DATATYPE_UINT64) {
                fprintf(outputFile, "%-40s%12lld\t%#08llx\n", raplData->event_names[i], values[i], values[i]);
            }
        }
    }

    free(values);
    free(raplData);
    fclose(outputFile);
}

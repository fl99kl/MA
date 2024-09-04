﻿#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <search.h> // Using search.h for hash table implementation

#define MAX_RAPL_EVENTS 64
#define HASH_TABLE_SIZE 256

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
    FILE *outputFile = fopen(output_file_path, "a");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }
    fprintf(outputFile, "Output start Debug\n");
    fclose(outputFile);
}

void outputEnd(const char* output_file_path) {
    FILE *outputFile = fopen(output_file_path, "a");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }
    fprintf(outputFile, "Output end Debug\n");
    fclose(outputFile);
}

void clearFile(const char* output_file_path) {
    FILE *outputFile = fopen(output_file_path, "w");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }
    fprintf(outputFile, "File cleared\n");
    fclose(outputFile);
}

void add1ToFile(const char* output_file_path) {
    FILE *outputFile = fopen(output_file_path, "a");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }
    fprintf(outputFile, "1\n");
    fclose(outputFile);
}

void add2ToFile(const char* output_file_path) {
    FILE *outputFile = fopen(output_file_path, "a");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }
    fprintf(outputFile, "2\n");
    fclose(outputFile);
}

RaplData* startRapl(const char* output_file_path) {
    FILE *outputFile = fopen(output_file_path, "a");
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

    // Initialize the hash table
    if (!hcreate(HASH_TABLE_SIZE)) {
        perror("Failed to create hash table");
        exit(1);
    }

    // Create a list to track unique keys
    char** key_list = malloc(raplData->num_events * sizeof(char*));
    int key_count = 0;

    // Accumulate energy measurements
    for (i = 0; i < raplData->num_events; i++) {
        if (strstr(raplData->units[i], "nJ")) {
            double energy_joules = (double)values[i] / 1.0e9;

            // Extract the category (e.g., PACKAGE_ENERGY, DRAM_ENERGY)
            char category[256];
            // Adjusted to extract the part after "rapl:::" and before the second colon
            sscanf(raplData->event_names[i], "rapl:::%[^:]", category);

            // Prepare the ENTRY
            ENTRY item;
            ENTRY *found_item;
            item.key = strdup(category); // Copy the category to avoid pointer issues
            item.data = NULL;

            // Search the hash table
            found_item = hsearch(item, FIND);
            if (found_item == NULL) {
                // Insert if not found
                double *new_sum = (double *)malloc(sizeof(double));
                *new_sum = energy_joules;
                item.data = new_sum;
                if (hsearch(item, ENTER) == NULL) {
                    perror("Failed to insert into hash table");
                    exit(1);
                }
                // Store the key in key_list
                key_list[key_count++] = item.key;
            } else {
                // Update the existing entry
                double *sum = (double *)found_item->data;
                *sum += energy_joules;
                free(item.key); // Free the duplicated key since it is not used
            }
        }
    }

    // Output the summed energy measurements
    fprintf(outputFile, "\nStopping measurements, took %.3fs, gathering results...\n\n", elapsed_time);
    fprintf(outputFile, "Scaled energy measurements:\n");

    // Iterate over the stored keys and print the results
    for (i = 0; i < key_count; i++) {
        ENTRY item;
        item.key = key_list[i];
        ENTRY *entry = hsearch(item, FIND);
        if (entry != NULL) {
            double *sum = (double *)entry->data;
            fprintf(outputFile, "%-40s%12.6f J\t(Average Power %.1fW)\n",
                   entry->key,
                   *sum,
                   *sum / elapsed_time);
            free(entry->data); // Free the allocated sum
        }
        free(key_list[i]); // Free the key
    }

    // Cleanup
    free(key_list);
    hdestroy();
    free(values);
    free(raplData);
    fclose(outputFile);
}
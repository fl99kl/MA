#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <search.h> // Using search.h for hash table implementation

#define MAX_RAPL_EVENTS 64
#define HASH_TABLE_SIZE 256
#define MAX_LINE_LENGTH 1024
#define MAX_TEST_CASES 100

typedef struct {
    char test_case_id[50];
    double duration; // duration in seconds
    double total_energy_consumed_package; // total energy consumed
    double average_energy_consumed_package;// average energy consumed (total_energy_consumed/duration)
    double total_energy_consumed_dram;
    double average_energy_consumed_dram;
    char timestamp[20];
} TestCase;

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

void addLineToFile(const char* output_file_path, const char* test_name) {
    FILE *outputFile = fopen(output_file_path, "a");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }
    fprintf(outputFile, "\n");
    fprintf(outputFile, "---------------------------\n");
    fprintf(outputFile, "%s\n", test_name);
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

    // Start Counting
    raplData->before_time = PAPI_get_real_nsec();
    retval = PAPI_start(raplData->EventSet);
    if (retval != PAPI_OK) handle_error(retval, outputFile);

    fclose(outputFile);
    return raplData;
}

TestCase readAndStopRapl(RaplData* raplData, const char* output_file_path, const char* test_case_name) {
    FILE *outputFile = fopen(output_file_path, "a");
    if (outputFile == NULL) {
        perror("Failed to open output file");
        exit(1);
    }

    TestCase testCase;
    strcpy(testCase.test_case_id, test_case_name);
    time_t now = time(NULL);
    strftime(testCase.timestamp, sizeof(testCase.timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    int retval, i;
    long long after_time;
    double elapsed_time;
    long long* values = calloc(raplData->num_events, sizeof(long long));
    if (values == NULL) handle_error(PAPI_ENOMEM, outputFile);

    // Stop Counting
    after_time = PAPI_get_real_nsec();
    retval = PAPI_stop(raplData->EventSet, values);
    if (retval != PAPI_OK) handle_error(retval, outputFile);

    elapsed_time = ((double)(after_time - raplData->before_time)) / 1.0e9;
    testCase.duration = elapsed_time;

    double total_energy_package = 0;
    double total_energy_dram = 0;

    // Initialize the hash table
    if (!hcreate(HASH_TABLE_SIZE)) {
        perror("Failed to create hash table");
        exit(1);
    }

    // Accumulate energy measurements
    for (i = 0; i < raplData->num_events; i++) {
        if (strstr(raplData->units[i], "nJ")) {
            double energy_joules = (double)values[i] / 1.0e9;

            // Extract the category (e.g., PACKAGE_ENERGY, DRAM_ENERGY)
            char category[256];
            // Extract the part after "rapl:::" and before the second colon
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

    testCase.total_energy_consumed_package = total_energy_package;
    testCase.average_energy_consumed_package = total_energy_package / elapsed_time;
    testCase.total_energy_consumed_dram = total_energy_dram;
    testCase.average_energy_consumed_dram = total_energy_dram / elapsed_time;

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

    return testCase;
}

int read_csv(const char *filename, TestCase test_cases[], int max_test_cases) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (count >= max_test_cases) break;

        // Parse the line and fill the TestCase struct
        sscanf(line, "%49[^,],%lf,%lf,%lf,%lf,%lf,%19[^\n]", 
               test_cases[count].test_case_id, 
               &test_cases[count].duration, 
               &test_cases[count].total_energy_consumed_package, 
               &test_cases[count].average_energy_consumed_package, 
               &test_cases[count].total_energy_consumed_dram, 
               &test_cases[count].average_energy_consumed_dram, 
               test_cases[count].timestamp);
        count++;
    }

    fclose(file);
    return count;
}

TestCase* find_test_case(TestCase test_cases[], int num_cases, const char *test_case_id) {
    for (int i = 0; i < num_cases; i++) {
        if (strcmp(test_cases[i].test_case_id, test_case_id) == 0) {
            return &test_cases[i];
        }
    }
    return NULL;
}

void write_csv(const char *filename, TestCase test_cases[], int num_cases) {
    FILE *file = fopen(filename, "w"); // Open the file in write mode to overwrite it
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < num_cases; i++) {
        fprintf(file, "%s,%.2f,%.2f,%.2f,%s\n",
                test_cases[i].test_case_id,
                test_cases[i].duration,
                test_cases[i].total_energy_consumed_package,
                test_cases[i].average_energy_consumed_package,
                test_cases[i].total_energy_consumed_dram,
                test_cases[i].average_energy_consumed_dram,
                test_cases[i].timestamp);
    }

    fclose(file);
}

void get_timestamp(char *buffer, int len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, len, "%Y-%m-%d %H:%M:%S", t);
}

void update_or_add_test_case(const char *filename, TestCase new_case) {
    TestCase test_cases[MAX_TEST_CASES];
    int num_cases = read_csv(filename, test_cases, MAX_TEST_CASES);

    TestCase *existing_case = find_test_case(test_cases, num_cases, new_case.test_case_id);
    if (existing_case) {
        // Update the existing test case
        printf("Updating existing test case: %s\n", existing_case->test_case_id);

        existing_case->duration = new_case.duration;
        existing_case->total_energy_consumed_package = new_case.total_energy_consumed_package;
        existing_case->average_energy_consumed_package = new_case.total_energy_consumed_package / new_case.duration;
        existing_case->total_energy_consumed_dram = new_case.total_energy_consumed_dram;
        existing_case->average_energy_consumed_dram = new_case.total_energy_consumed_dram / new_case.duration;
        get_timestamp(existing_case->timestamp, sizeof(existing_case->timestamp));
    } else {
        // Add the new test case to the array
        printf("Adding new test case: %s\n", new_case.test_case_id);
        get_timestamp(new_case.timestamp, sizeof(new_case.timestamp));

        test_cases[num_cases++] = new_case;
    }

    // Write the updated test cases back to the CSV file
    write_csv(filename, test_cases, num_cases);
}
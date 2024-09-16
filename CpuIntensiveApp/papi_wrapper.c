#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <search.h> // Using search.h for hash table implementation
#include <sys/time.h>
#include <curl/curl.h>

#define MAX_RAPL_EVENTS 64
#define HASH_TABLE_SIZE 256
#define MAX_LINE_LENGTH 1024
#define MAX_TEST_CASES 100

typedef struct {
    char test_case_id[50];
    double duration; // duration in seconds
    double total_energy_consumed_package; // total energy consumed
    double average_energy_consumed_package;// average energy consumed (total_energy_consumed/duration)
    double median_energy_consumed_package;// average energy consumed (total_energy_consumed/duration)
    double total_energy_consumed_dram;
    double average_energy_consumed_dram;
    double median_energy_consumed_dram;
    char timestamp[20];
} TestCase;

typedef struct {
    double total_energy_consumed_package; // total energy consumed
    double total_energy_consumed_dram;
    char timestamp[20];
} IntermediateTestCase;

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

    printf("PAPI stop\n");

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

    // Create a list to track unique keys
    char** key_list = malloc(raplData->num_events * sizeof(char*));
    int key_count = 0;

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

            if (strcmp(category, "PACKAGE_ENERGY") == 0) {
                total_energy_package += energy_joules;
            } else if (strcmp(category, "DRAM_ENERGY") == 0) {
                total_energy_dram += energy_joules;
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

IntermediateTestCase getIntermediateRaplResults(RaplData* raplData, double before_total_package, double before_total_dram) {
    IntermediateTestCase intermediate_test_case;
    
    // Get the current timestamp
    time_t now = time(NULL);
    strftime(intermediate_test_case.timestamp, sizeof(intermediate_test_case.timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    int retval, i;
    long long* values = calloc(raplData->num_events, sizeof(long long));
    if (values == NULL) {
        perror("Failed to allocate memory for values");
        exit(1);
    }

    // Read current energy counters without stopping the measurement
    retval = PAPI_read(raplData->EventSet, values);

    if (retval != PAPI_OK) {
        perror("Failed to read PAPI events");
        free(values);
        exit(1);
    }

    double total_energy_package = 0;
    double total_energy_dram = 0;

    // Accumulate energy measurements
    for (i = 0; i < raplData->num_events; i++) {
        if (strstr(raplData->units[i], "nJ")) {
            double energy_joules = (double)values[i] / 1.0e9;
            char category[256];
            sscanf(raplData->event_names[i], "rapl:::%[^:]", category);

            if (strcmp(category, "PACKAGE_ENERGY") == 0) {
                total_energy_package += energy_joules;
            } else if (strcmp(category, "DRAM_ENERGY") == 0) {
                total_energy_dram += energy_joules;
            }
        }
    }
    //total_energy_package = total_energy_package - before_total_package;
    //total_energy_dram = total_energy_dram - before_total_dram;
    
    // Store energy data in the testCase object
    intermediate_test_case.total_energy_consumed_package = total_energy_package;
    intermediate_test_case.total_energy_consumed_dram = total_energy_dram;

    // Clean up
    free(values);

    // Return the populated testCase object with the intermediate data
    return intermediate_test_case;
}


int read_csv(const char *filename, TestCase test_cases[], int max_test_cases) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    // Skip the header line
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        if (count >= max_test_cases) break;

        // Parse the line and fill the TestCase struct
        sscanf(line, "%49[^,],%lf,%lf,%lf,%lf,%lf,%lf,%lf,%19[^\n]", 
               test_cases[count].test_case_id, 
               &test_cases[count].duration, 
               &test_cases[count].total_energy_consumed_package, 
               &test_cases[count].average_energy_consumed_package, 
               &test_cases[count].median_energy_consumed_package, 
               &test_cases[count].total_energy_consumed_dram, 
               &test_cases[count].average_energy_consumed_dram,
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
    fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
        "testCase Name",
        "testCase duration",
        "total from package",
        "average from package",
        "median from package",
        "total from dram",
        "average from dram",
        "median from dram",
        "timestamp");

    for (int i = 0; i < num_cases; i++) {
        
        printf("duration: %.4f\n", test_cases[i].total_energy_consumed_dram);
        printf("new_case.total_energy_consumed_dram: %.4f\n", test_cases[i].average_energy_consumed_dram);
        printf("average energy dram: %.4f\n", median_energy_consumed_dram);
        printf("-------------------\n");
        fprintf(file, "%s,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%s\n",
                test_cases[i].test_case_id,
                test_cases[i].duration,
                test_cases[i].total_energy_consumed_package,
                test_cases[i].average_energy_consumed_package,
                test_cases[i].median_energy_consumed_package,
                test_cases[i].total_energy_consumed_dram,
                test_cases[i].average_energy_consumed_dram,
                test_cases[i].median_energy_consumed_dram,
                test_cases[i].timestamp);
    }

    fclose(file);
}

void get_timestamp(char *buffer, int len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, len, "%Y-%m-%d %H:%M:%S", t);
}

void updateOrAddTestCase(const char *filename, TestCase new_case) {
    TestCase test_cases[MAX_TEST_CASES];
    int num_cases = read_csv(filename, test_cases, MAX_TEST_CASES);

    TestCase *existing_case = find_test_case(test_cases, num_cases, new_case.test_case_id);
    if (existing_case) {
        // Update the existing test case

        existing_case->duration = new_case.duration;
        existing_case->total_energy_consumed_package = new_case.total_energy_consumed_package;
        existing_case->average_energy_consumed_package = new_case.total_energy_consumed_package / new_case.duration;
        existing_case->median_energy_consumed_package = new_case.median_energy_consumed_package;
        existing_case->total_energy_consumed_dram = new_case.total_energy_consumed_dram;
        
        existing_case->average_energy_consumed_dram = new_case.total_energy_consumed_dram / new_case.duration;
        existing_case->median_energy_consumed_dram = new_case.median_energy_consumed_dram;
        get_timestamp(existing_case->timestamp, sizeof(existing_case->timestamp));
    } else {
        // Add the new test case to the array
        get_timestamp(new_case.timestamp, sizeof(new_case.timestamp));

        test_cases[num_cases++] = new_case;
    }

    // Write the updated test cases back to the CSV file
    write_csv(filename, test_cases, num_cases);
}

void addTsdbEntry(TestCase new_case) {
    // Ensure the data buffer is large enough for the line protocol
    char data[512];

    // Prepare data in InfluxDB line protocol format:
    // unit_test_energy,test_name=UnitTest1 duration=5.2,avg_energy_pkg=10.5,total_energy_pkg=50.0,avg_energy_dram=3.2,total_energy_dram=16.0 <timestamp>
    snprintf(data, sizeof(data),
             "unit_test_energy,test_name=%s duration=%.4f,total_energy_pkg=%.4f,avg_energy_pkg=%.4f,median_energy_pkg=%.4f,total_energy_dram=%.4f,avg_energy_dram=%.4f,median_energy_dram=%.4f",
             new_case.test_case_id, new_case.duration, new_case.total_energy_consumed_package, new_case.average_energy_consumed_package, new_case.median_energy_consumed_package, new_case.total_energy_consumed_dram, new_case.average_energy_consumed_dram, new_case.median_energy_consumed_dram);

    // Initialize libcurl
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();  // Initialize a curl session
    if(curl) {
        // Set the URL for InfluxDB API (change the URL to match your setup)
        const char *url = "http://localhost:8086/api/v2/write?bucket=myBucket&org=MA";

        // Set headers including the authorization token (replace with your InfluxDB token)
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: text/plain");
        headers = curl_slist_append(headers, "Authorization: Token ppaJ5zlrWXA4CKbZsCSwwIRjbffgSVbKyQxEWWzb9wY3HTPiD6S7d66FaomiCiTqDXQQrJY_vXFxqDBUoY4rtg==");

        // Set the URL, headers, and data (line protocol)
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Cleanup curl session
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);  // Clean up the headers list
    }
}

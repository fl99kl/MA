#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <curl/curl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <search.h> // Using search.h for hash table implementation
#include <sys/time.h>

#define URL "http://localhost:8086/api/v2/write?org=MA&bucket=myBucket&precision=s"
#define TOKEN "ppaJ5zlrWXA4CKbZsCSwwIRjbffgSVbKyQxEWWzb9wY3HTPiD6S7d66FaomiCiTqDXQQrJY_vXFxqDBUoY4rtg=="
#define TEST_CASE_NAME "idle_consumption"

#define MAX_RAPL_EVENTS 64
#define HASH_TABLE_SIZE 256
#define MAX_LINE_LENGTH 1024
#define MAX_TEST_CASES 100

typedef struct {
    int EventSet;
    long long before_time;
    int num_events;
    char event_names[MAX_RAPL_EVENTS][PAPI_MAX_STR_LEN];
    char units[MAX_RAPL_EVENTS][PAPI_MIN_STR_LEN];
    int data_type[MAX_RAPL_EVENTS];
} RaplData;

// Function to send data to InfluxDB
int send_to_influxdb(const char* data) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL and other necessary options
        curl_easy_setopt(curl, CURLOPT_URL, URL);

        // Set the authorization token
        headers = curl_slist_append(headers, "Authorization: Token " TOKEN);
        headers = curl_slist_append(headers, "Content-Type: text/plain");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the POST data (InfluxDB line protocol format)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
    return (int)res;
}

// Function to perform PAPI measurements
void papi_measurement() {
    long long after_time;
    double elapsed_time;
    int retval, i, cid, rapl_cid = -1, numcmp;
    int code, r;
    double avg_pkg_power, avg_dram_power;
    RaplData* raplData = (RaplData*)malloc(sizeof(RaplData));
    const PAPI_component_info_t *cmpinfo = NULL;
    PAPI_event_info_t evinfo;
    raplData->EventSet = PAPI_NULL;
    raplData->num_events = 0;

    // PAPI event setup (for package and DRAM energy)
    int event_set = PAPI_NULL;
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI library init error!\n");
        exit(1);
    }

    numcmp = PAPI_num_components();

    for (cid = 0; cid < numcmp; cid++) {
        if ((cmpinfo = PAPI_get_component_info(cid)) == NULL) {
            fprintf(stderr, "PAPI_get_component_info failed\n");
            exit(1);
        }

        if (strstr(cmpinfo->name, "rapl")) {
            rapl_cid = cid;

            if (cmpinfo->disabled) {
                fprintf(stderr, "RAPL component disabled\n");
                exit(1);
            }
            break;
        }
    }

    // Component not found
    if (cid == numcmp) {
        fprintf(stderr, "No rapl component found\n");
        exit(1);
    }

    // Create an EventSet
    retval = PAPI_create_eventset(&(raplData->EventSet));

    // Add all events
    code = PAPI_NATIVE_MASK;
    r = PAPI_enum_cmp_event(&code, PAPI_ENUM_FIRST, rapl_cid);

    
    while (r == PAPI_OK) {
        retval = PAPI_event_code_to_name(code, raplData->event_names[raplData->num_events]);
        if (retval != PAPI_OK) {
            fprintf(stderr, "Error translating %#x\n", code);
        }

        retval = PAPI_get_event_info(code, &evinfo);

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

    // Simulate workload (replace this with actual workload)
    sleep(10);

    long long* values = calloc(raplData->num_events, sizeof(long long));

    // Stop Counting
    after_time = PAPI_get_real_nsec();
    retval = PAPI_stop(raplData->EventSet, values);

    elapsed_time = ((double)(after_time - raplData->before_time)) / 1.0e9;

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
    avg_pkg_power = total_energy_package / elapsed_time;
    avg_dram_power = total_energy_dram / elapsed_time;

    // Cleanup
    free(key_list);
    hdestroy();
    free(values);
    free(raplData);

    // Debug print statements for energy consumption
    printf("Test Case: %s\n", TEST_CASE_NAME);
    printf("Average Package Power: %lf Watts\n", avg_pkg_power);
    printf("Total Package Energy: %lf Joules\n", total_energy_package);
    printf("Average DRAM Power: %lf Watts\n", avg_dram_power);
    printf("Total DRAM Energy: %lf Joules\n", total_energy_dram);

    // Format data for InfluxDB (line protocol)
    char influx_data[512];
    snprintf(influx_data, sizeof(influx_data),
             "unit_test_energy,test_name=%s duration=%.4f,total_energy_pkg=%.4f,avg_energy_pkg=%.4f,median_energy_pkg=%.4f,total_energy_dram=%.4f,avg_energy_dram=%.4f,median_energy_dram=%.4f",
             TEST_CASE_NAME, elapsed_time, total_energy_package, avg_pkg_power, 0.0, total_energy_dram, avg_dram_power, 0.0);

    // Send the data to InfluxDB
    send_to_influxdb(influx_data);
}

int main() {
    // Run the PAPI measurement and send data to InfluxDB
    papi_measurement();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <curl/curl.h>

#define URL "http://localhost:8086/api/v2/write?org=MA&bucket=myBucket&precision=s"
#define TOKEN "ppaJ5zlrWXA4CKbZsCSwwIRjbffgSVbKyQxEWWzb9wY3HTPiD6S7d66FaomiCiTqDXQQrJY_vXFxqDBUoY4rtg=="
#define TEST_CASE_NAME "idle_consumption"

// Function to send data to InfluxDB
int send_to_influxdb(const char* data) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
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
    long long before_time,after_time;
    double elapsed_time;
    int retval;
    long long start_values[2], end_values[2];
    double avg_pkg_power, total_pkg_energy, avg_dram_power, total_dram_energy;

    // PAPI event setup (for package and DRAM energy)
    int event_set = PAPI_NULL;
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI library init error!\n");
        exit(1);
    }

    PAPI_create_eventset(&event_set);
    PAPI_add_event(event_set, PAPI_ENERGY_PKG);
    PAPI_add_event(event_set, PAPI_ENERGY_DRAM);

    before_time=PAPI_get_real_nsec();

    // Start PAPI measurements
    PAPI_start(event_set);
    PAPI_read(event_set, start_values);

    // Simulate workload (replace this with actual workload)
    sleep(2);

    // Stop PAPI measurements
    PAPI_stop(event_set, end_values);
    after_time=PAPI_get_real_nsec();
    elapsed_time=((double)(after_time-before_time))/1.0e9;

    // Calculate energy metrics
    total_pkg_energy = (end_values[0] - start_values[0]) / 1e9; // Convert from nanojoules to joules
    total_dram_energy = (end_values[1] - start_values[1]) / 1e9;

    avg_pkg_power = total_pkg_energy / elapsed_time;
    avg_dram_power = total_dram_energy / elapsed_time;

    // Format data for InfluxDB (line protocol)
    char influx_data[256];
    snprintf(influx_data, sizeof(influx_data),
             "energy_measurement,test_case=%s avg_pkg_power=%lf,total_pkg_energy=%lf,avg_dram_power=%lf,total_dram_energy=%lf",
             TEST_CASE_NAME, avg_pkg_power, total_pkg_energy, avg_dram_power, total_dram_energy);

    // Send the data to InfluxDB
    send_to_influxdb(influx_data);
}

int main() {
    // Run the PAPI measurement and send data to InfluxDB
    papi_measurement();
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <sys/time.h>

int main() {
    char *test_name = "UnitTest1";   // Test case name
    float duration = 5.2;            // Test duration in seconds
    float avg_energy_pkg = 10.5;     // Average package energy consumption
    float total_energy_pkg = 50.0;   // Total package energy consumption
    float avg_energy_dram = 3.2;     // Average DRAM energy consumption
    float total_energy_dram = 16.0;  // Total DRAM energy consumption

    // Get the current time in nanoseconds (to be used as the timestamp)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long timestamp = (long long)tv.tv_sec * 1000000000LL + (long long)tv.tv_usec * 1000LL;

    // Ensure the data buffer is large enough for the line protocol
    char data[512];

    // Prepare data in InfluxDB line protocol format:
    // unit_test_energy,test_name=UnitTest1 duration=5.2,avg_energy_pkg=10.5,total_energy_pkg=50.0,avg_energy_dram=3.2,total_energy_dram=16.0 <timestamp>
    snprintf(data, sizeof(data),
             "unit_test_energy,test_name=%s duration=%f,avg_energy_pkg=%f,total_energy_pkg=%f,avg_energy_dram=%f,total_energy_dram=%f %lld",
             test_name, duration, avg_energy_pkg, total_energy_pkg, avg_energy_dram, total_energy_dram, timestamp);

    // Print the resulting string to check the output
    printf("Formatted data: %s\n", data);

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

    return 0;
}

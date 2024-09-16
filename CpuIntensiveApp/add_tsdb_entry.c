#include <stdio.h>
#include <string.h>
#include <curl/curl.h>  // Include curl.h for libcurl functionality
#include <time.h>
#include <sys/time.h>

int main() {
    char *key = "sensor1";
    float value1 = 23.5;
    float value2 = 45.6;

    // Get the current time in seconds and microseconds
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Convert the current time to nanoseconds
    long long timestamp = (long long)tv.tv_sec * 1000000000LL + (long long)tv.tv_usec * 1000LL;

    // Ensure the data buffer is large enough
    char data[256];

    // Format string using %lld for long long timestamp
    snprintf(data, sizeof(data), "my_measurement,key=%s value1=%f,value2=%f %lld", key, value1, value2, timestamp);

    // Print the formatted string to check the output
    printf("Formatted data: %s\n", data);

    // Initialize libcurl
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();  // Initialize a curl session
    if(curl) {
        // Set the URL for InfluxDB API (change the URL to match your setup)
        const char *url = "http://localhost:8086/api/v2/write?bucket=myBucket&org=MA";

        // Set headers including the authorization token (replace with your token)
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: text/plain");
        headers = curl_slist_append(headers, "Authorization: Token ppaJ5zlrWXA4CKbZsCSwwIRjbffgSVbKyQxEWWzb9wY3HTPiD6S7d66FaomiCiTqDXQQrJY_vXFxqDBUoY4rtg==");

        // Set the URL and headers for the request
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the data to send (line protocol)
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

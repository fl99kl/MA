#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/time.h>

const char *url = "http://localhost:8086/api/v2/write";
const char *bucket = "myBucket";
const char *org = "MA";
const char *token = "ppaJ5zlrWXA4CKbZsCSwwIRjbffgSVbKyQxEWWzb9wY3HTPiD6S7d66FaomiCiTqDXQQrJY_vXFxqDBUoY4rtg==";  // Replace with your token

char *key = "sensor1";
float value1 = 23.5;
float value2 = 45.6;



// Get the current time in seconds and microseconds
struct timeval tv;
gettimeofday(&tv, NULL);
long long timestamp = (long long)tv.tv_sec * 1000000000LL + (long long)tv.tv_usec * 1000LL;

char data[256];
snprintf(data, sizeof(data), "my_measurement,key=%s value1=%f,value2=%f %lld", key, value1, value2, timestamp);

CURL *curl;
CURLcode res;

curl = curl_easy_init();
if(curl) {
    char url_with_params[512];
    snprintf(url_with_params, sizeof(url_with_params), "%s?bucket=%s&org=%s", url, bucket, org);

    // Set URL and token for authorization
    curl_easy_setopt(curl, CURLOPT_URL, url_with_params);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: text/plain");
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Token %s", token);
    headers = curl_slist_append(headers, auth_header);

    // Set HTTP headers
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the data to send (line protocol)
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

    // Perform the request
    res = curl_easy_perform(curl);

    // Check for errors
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    // Cleanup
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
}


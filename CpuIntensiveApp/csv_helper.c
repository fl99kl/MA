#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LENGTH 1024
#define MAX_TEST_CASES 100

typedef struct {
    char test_case_id[50];
    double duration; // duration in seconds
    double total_energy_consumed; // total energy consumed
    double average_energy_consumed; // average energy consumed (total_energy_consumed/duration)
    char timestamp[20];
} TestCase;

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
        sscanf(line, "%49[^,],%lf,%lf,%lf,%19[^\n]", 
               test_cases[count].test_case_id, 
               &test_cases[count].duration, 
               &test_cases[count].total_energy_consumed, 
               &test_cases[count].average_energy_consumed, 
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
                test_cases[i].total_energy_consumed,
                test_cases[i].average_energy_consumed,
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
        existing_case->total_energy_consumed = new_case.total_energy_consumed;
        existing_case->average_energy_consumed = new_case.total_energy_consumed / new_case.duration;
        get_timestamp(existing_case->timestamp, sizeof(existing_case->timestamp));
    } else {
        // Add the new test case to the array
        printf("Adding new test case: %s\n", new_case.test_case_id);
        new_case.average_energy_consumed = new_case.average_energy_consumed;
        get_timestamp(new_case.timestamp, sizeof(new_case.timestamp));

        test_cases[num_cases++] = new_case;
    }

    // Write the updated test cases back to the CSV file
    write_csv(filename, test_cases, num_cases);
}

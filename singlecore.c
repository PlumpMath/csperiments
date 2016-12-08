#include <stdlib.h>
#include <stdio.h>

#define TEST_CASES 1000000
#define TEST_RUNS 30

int
compare(int* a, int* b)
{
    return *a - *b;
}

int
main(void)
{
    fprintf(stderr, "Starting Test\n");
    srand(time(NULL));

    int* num_buffer = malloc(sizeof(int) * TEST_CASES);
    int num_count = 0;

    for (int run=0; run<TEST_RUNS; run++) {
        // Random numbers in (0 to RAND_MAX)
        for (int i=0; i<TEST_CASES; i++) {
            num_buffer[num_count++] = rand();
        }

        fprintf(stderr, "Running test case: %i\n", run);

        // Serial Version
        // Sort them
        qsort(num_buffer, num_count, sizeof(int), compare);

        // Print them
        for (int i=0; i<num_count; i++) {
            printf("%i,", num_buffer[i]);
        }

        num_count = 0;
    }
    fprintf(stderr, "Done\n");
}

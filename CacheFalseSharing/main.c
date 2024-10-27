#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define NUM_THREADS 4
#define ITERATIONS 100000000

int shared_array[NUM_THREADS];

typedef struct {
    int thread_id;
} thread_data_t;

double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void *direct_increment(void *args) {
    thread_data_t *data = (thread_data_t *)args;
    int idx = data->thread_id;

    for (int iter = 0; iter < ITERATIONS; ++iter) {
        shared_array[idx]++;
    }
    return NULL;
}

void *local_then_single_write(void *args) {
    thread_data_t *data = (thread_data_t *)args;
    int local_copy = 0;

    for (int iter = 0; iter < ITERATIONS; ++iter) {
        local_copy++;
    }

    shared_array[data->thread_id] = local_copy;
    return NULL;
}

void run_experiment(void *(*func)(void *), const char *description) {
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        shared_array[i] = 0;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
    }

    double start_time = get_time();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, func, (void*)&thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end_time = get_time();
    printf("%s Time: %f seconds\n", description, end_time - start_time);

    for (int i = 0; i < NUM_THREADS; i++) {
        printf("shared_array[%d] = %d\n", i, shared_array[i]);
    }
}

int main(void) {
    srand((uint)time(NULL));
    run_experiment(direct_increment, "Direct Increment (False Sharing)");
    run_experiment(local_then_single_write, "Local Increment then Single Write (Reduced False Sharing)");
    return 0;
}

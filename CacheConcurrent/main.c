#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <sys/time.h>

#define NUM_THREADS 4
#define ITERATIONS 10000000

int shared_normal = 0;
atomic_int shared_atomic = 0;
atomic_int shared_atomic_relaxed = 0;

double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void *regular_increment(void *args) {
    for (int i = 0; i < ITERATIONS; ++i) {
        shared_normal++;
    }
    return NULL;
}

void *atomic_increment(void *args) {
    for (int i = 0; i < ITERATIONS; ++i) {
        atomic_fetch_add(&shared_atomic, 1);
    }
    return NULL;
}

void *atomic_increment_relaxed(void *args) {
    for (int i = 0; i < ITERATIONS; ++i) {
        atomic_fetch_add_explicit(&shared_atomic_relaxed, 1, memory_order_relaxed);
    }
    return NULL;
}

void run_experiment(void *(*func)(void *), const char *description) {
    pthread_t threads[NUM_THREADS];

    double start_time = get_time();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, func, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end_time = get_time();
    printf("%s Time: %f seconds\n", description, end_time - start_time);

    if (func == regular_increment) {
        printf("Final shared_normal value: %d\n", shared_normal);
    } else if (func == atomic_increment) {
        printf("Final shared_atomic value: %d\n", atomic_load(&shared_atomic));
    } else if (func == atomic_increment_relaxed) {
        printf("Final shared_atomic_relaxed value: %d\n", atomic_load(&shared_atomic_relaxed));
    }
}

int main(void) {
    srand((uint)time(NULL));
    run_experiment(regular_increment, "Regular Increment");
    run_experiment(atomic_increment_relaxed, "Atomic Increment (Relaxed Memory Order)");
    run_experiment(atomic_increment, "Atomic Increment");
    return 0;
}

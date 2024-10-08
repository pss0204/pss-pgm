//버전 3는 부모 자식 없음
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 2



void test_race() {
    register int x5 asm ("x5") = 1;
    register int x6 asm ("x6") = 2;
    register int x7 asm ("x7") = 3;

    asm volatile (
        ".word 0b00000000011100101001001011111011    # TEST_RACE x6, x5, x7"
        : "+r"(x6)
        : "r"(x5), "r"(x7)
    );
}

void* thread_function(void* arg) {
    test_race();
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, NULL) != 0) {
            perror("pthread_create failed");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
            return 1;
        }
    }

    printf("All threads completed\n");
    return 0;
}
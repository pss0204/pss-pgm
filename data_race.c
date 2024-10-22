#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define NUM_ITERATIONS 1000000

int counter = 0;

void* increment(void* arg) {
    for(int i = 0; i < NUM_ITERATIONS; i++) {
        counter++; // 데이터 레이스 발생 지점
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // 두 개의 스레드 생성
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment, NULL);
    }

    // 모든 스레드가 종료될 때까지 대기
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("최종 카운터 값: %d\n", counter);
    return 0;
}
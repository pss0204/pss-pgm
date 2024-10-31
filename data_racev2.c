#include <stdio.h>
#include <pthread.h>
//산술 연산 만으로 레이스
#define NUM_THREADS 2
#define NUM_ITERATIONS 1000000

volatile int shared_var = 0;

void* thread_func(void* arg) {
    for(int i = 0; i < NUM_ITERATIONS; i++) {
        __asm__ __volatile__(
            "add t0, %1, zero\n\t"     // shared_var 값을 t0에 로드
            "addi t0, t0, 1\n\t"       // t0에 1을 더함
            "add %0, t0, zero\n\t"     // t0 값을 shared_var에 저장
            : "=r" (shared_var)        // 출력: shared_var
            : "r" (shared_var)         // 입력: shared_var
            : "t0"                     // 사용된 임시 레지스터
        );
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // 두 개의 스레드 생성
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }

    // 모든 스레드가 종료될 때까지 대기
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // 모든 과정이 끝난 후 TEST_RACE 명령어 실행
    __asm__ __volatile__(
        ".word 0b00000000011100101001001011111011    # TEST_RACE x6, x5, x7"
        :
        :
        : "memory"
    );

    printf("최종 값: %d\n", shared_var);
    return 0;
}
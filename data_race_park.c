#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define NUM_ITERATIONS 1000000

volatile int counter = 0;

// 어셈블리 레이블을 함수처럼 선언하지 않고 직접 레이블 정의
#define TEST_RACE_LABEL do { \
    __asm__ __volatile__ (".globl TEST_RACE\n" \
                          "TEST_RACE:"); \
} while(0)

#define PARK_LABEL do { \
    __asm__ __volatile__ (".globl PARK\n" \
                          "PARK:"); \
} while(0)

void* increment(void* arg) {
    for(int i = 0; i < NUM_ITERATIONS; i++) {
        __asm__ __volatile__(
            "lw t0, 0(%0)\n\t"    // counter 값을 t0 레지스터로 로드
            "addi t0, t0, 1\n\t"  // t0 값에 1을 더함
            "sw t0, 0(%0)"        // t0 값을 다시 counter에 스토어
            :
            : "r" (&counter)       // counter의 주소를 입력으로 사용
            : "t0", "memory"
        ); // 인라인 어셈블리를 사용한 직접 로드/스토어 데이터 레이스 발생 지점
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // TEST_RACE 레이블 삽입
   
    PARK_LABEL;

    // 두 개의 스레드 생성
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment, NULL);
    }

    // 모든 스레드가 종료될 때까지 대기
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // PARK 레이블 삽입
     TEST_RACE_LABEL;

    printf("최종 카운터 값: %d\n", counter);
    return 0;
}
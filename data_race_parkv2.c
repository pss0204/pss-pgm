#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define NUM_ITERATIONS 1000000

volatile int counter = 0;

void* increment(void* arg) {
    for(int i = 0; i < NUM_ITERATIONS; i++) {
        __asm__ __volatile__(
            "la t1, counter\n\t"   // counter의 주소를 t1에 로드
            "lw t0, 0(t1)\n\t"     // counter 값을 t0에 로드
            "addi t0, t0, 1\n\t"   // t0 값 증가
            "sw t0, 0(t1)"         // 증가된 값을 counter에 저장
            :
            : 
            : "t0", "t1", "memory"
        );
    }
    return NULL;
}

int main() {
    // 첫 번째 PARK
    __asm__ __volatile__(".word 0b11111111110011101000111100001011\n\t");
    
    pthread_t threads[NUM_THREADS];
    
    // 여기서 counter 변수를 직접 수정하는 코드
    __asm__ __volatile__(
        "la t1, counter\n\t"      // counter 주소를 t1에 로드
        "lw t0, 0(t1)\n\t"         // counter 값을 읽음
        "nop\n\t"                 // 의도적 지연
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "addi t0, t0, 1\n\t"      // 값 증가
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "sw t0, 0(t1)\n\t"         // 값 저장
        :
        : 
        : "t0", "t1", "memory"
    );

    // 스레드 생성 및 실행
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment, NULL);
    }

    // 스레드 종료 대기
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // 두 번째 PARK와 TEST_RACE
    __asm__ __volatile__(
        ".word 0b11111111110011101000111100001011\n\t"    // PARK
        ".word 0b00000000011100101001001011111011\n\t"    // TEST_RACE
        :
        :
        : "memory"
    );

    printf("최종 카운터 값: %d\n", counter);
    return 0;
}
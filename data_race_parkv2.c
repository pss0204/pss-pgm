#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define NUM_ITERATIONS 1000000

volatile int counter = 0;

void* increment(void* arg) {
    for(int i = 0; i < NUM_ITERATIONS; i++) {
        __asm__ __volatile__(
            "la t1, counter\n\t"   // counter의 주소를 t1에 로드
            "lw t0, 0(a5)\n\t"     // counter 값을 t0에 로드
            "addi t0, t0, 1\n\t"   // t0 값 증가
            "sw t0, 0(a5)"         // 증가된 값을 counter에 저장
            :
            : 
            : "t0", "t1", "memory"
        );
    }
    return NULL;
}

int main() {
    __asm__ __volatile__(".word 0b11111111110011101000111100001011   # PARK x28, x29, x30\n\t");
    pthread_t threads[NUM_THREADS];

    // 테스트용 카운터 증가 (메인 쓰레드에서 한번 실행)
    for(int i = 0; i < 10; i++) {  // 테스트용으로 10번만 실행
        __asm__ __volatile__(
            "lw t0, 0(sp)\n\t"    
            "addi t0, t0, 1\n\t"  
            "sw t0, 0(sp)"        
            :
            : "r" (&counter)
            : "t0", "memory"
        );
    }

    // 두 개의 스레드 생성
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment, NULL);
    }

    // 모든 스레드가 종료될 때까지 대기
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // 모든 과정이 끝난 후 TEST_RACE 명령어 실행
    __asm__ __volatile__(
        ".word 0b11111111110011101000111100001011   # PARK x28, x29, x30\n\t"
        ".word 0b00000000011100101001001011111011    # TEST_RACE x6, x5, x7"
         
        :
        :
        : "memory"
    );

    printf("최종 카운터 값: %d\n", counter);
    return 0;
}
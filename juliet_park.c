#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N_ITERS 1000000

// 전역 변수 정의
static volatile int gBadInt = 0;
static volatile int gGoodInt = 0;

// 뮤텍스 초기화
pthread_mutex_t gGoodLock;

// 취약한 함수: 락 없이 전역 변수 증가
void* helperBad(void* args) {
    for(int i = 0; i < N_ITERS; i++) {
       
        gBadInt++;
    }
    return NULL;
}

// 안전한 함수: 뮤텍스를 사용하여 전역 변수 증가
void* helperGood(void* args) {
    pthread_mutex_lock(&gGoodLock);
    for(int i = 0; i < N_ITERS; i++) {
        gGoodInt++;
    }
    pthread_mutex_unlock(&gGoodLock);
    return NULL;
}

// 취약한 시나리오 실행 함수
void CWE366_Race_Condition_Within_Thread_bad() {
    pthread_t threadA, threadB;

    gBadInt = 0;

    // 두 개의 스레드 생성
    if(pthread_create(&threadA, NULL, helperBad, NULL) != 0) {
        perror("pthread_create failed for threadA");
        exit(1);
    }
    if(pthread_create(&threadB, NULL, helperBad, NULL) != 0) {
        perror("pthread_create failed for threadB");
        exit(1);
    }

    // 스레드 종료 대기
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    printf("Bad Implementation - gBadInt 최종 값: %d (예상: %d)\n", gBadInt, 2 * N_ITERS);
}

// 안전한 시나리오 실행 함수
void CWE366_Race_Condition_Within_Thread_good() {
    pthread_t threadA, threadB;

    gGoodInt = 0;

    // 뮤텍스 초기화
    if(pthread_mutex_init(&gGoodLock, NULL) != 0) {
        perror("pthread_mutex_init failed");
        exit(1);
    }

    // 두 개의 스레드 생성
    if(pthread_create(&threadA, NULL, helperGood, NULL) != 0) {
        perror("pthread_create failed for threadA");
        exit(1);
    }
    if(pthread_create(&threadB, NULL, helperGood, NULL) != 0) {
        perror("pthread_create failed for threadB");
        exit(1);
    }

    // 스레드 종료 대기
    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    // 뮤텍스 파괴
    pthread_mutex_destroy(&gGoodLock);

    printf("Good Implementation - gGoodInt 최종 값: %d (예상: %d)\n", gGoodInt, 2 * N_ITERS);
}

int main(int argc, char *argv[]) {
    srand((unsigned)time(NULL));

    printf("=== CWE366 Race Condition Demonstration ===\n\n");

    // 안전한 구현 실행
    printf("안전한 구현 실행 (Good Implementation)...\n");
    CWE366_Race_Condition_Within_Thread_good();
    printf("\n");

    // 취약한 구현 실행
    printf("취약한 구현 실행 (Bad Implementation)...\n");
    CWE366_Race_Condition_Within_Thread_bad();
    printf("\n");

    // TEST_RACE 명령어 삽입
    __asm__ __volatile__(".word 0b00000000011100101001001011111011\n\t" );

    return 0;
}
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// 공유 변수
volatile int shared_var = 0;

// 동기화를 위한 뮤텍스와 조건 변수
pthread_mutex_t lock;
pthread_cond_t cond;

// 플래그 변수
int ready = 0;

// 스레드 1: 로드 연산 수행
void* thread_load(void* arg) {
    // 뮤텍스 잠금
    pthread_mutex_lock(&lock);
    
    // 준비 신호 대기
    while (!ready) {
        pthread_cond_wait(&cond, &lock);
    }
    
    // 공유 변수 읽기
    int value = shared_var;
    printf("Thread Load: 읽은 값 = %d\n", value);
    
    // 뮤텍스 해제
    pthread_mutex_unlock(&lock);
    return NULL;
}

// 스레드 2: 스토어 연산 수행
void* thread_store(void* arg) {
    // 뮤텍스 잠금
    pthread_mutex_lock(&lock);
    
    // 준비 신호 설정 및 조건 변수 신호
    ready = 1;
    pthread_cond_signal(&cond);
    
    // 공유 변수에 쓰기
    shared_var = 42;
    printf("Thread Store: 값 쓰기 = 42\n");
    
    // 컨플릭트 유도를 위해 잠시 대기
    usleep(1000); // 1밀리초 대기
    
    // 뮤텍스 해제
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t loader, storer;
    
    // 뮤텍스와 조건 변수 초기화
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);
    
    // 스레드 생성
    pthread_create(&loader, NULL, thread_load, NULL);
    pthread_create(&storer, NULL, thread_store, NULL);
    
    // 스레드 종료 대기
    pthread_join(loader, NULL);
    pthread_join(storer, NULL);
    
    // 뮤텍스와 조건 변수 파괴
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    
    return 0;
}
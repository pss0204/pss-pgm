#include <stdio.h>
#include <pthread.h>

volatile int shared_var = 0;

void* thread_function(void* arg) {
    // 스레드 1: 읽기 연산 수행
    int local = shared_var;
    return NULL;
}

int main() {
    pthread_t thread;
    
    // 스레드 생성
    pthread_create(&thread, NULL, thread_function, NULL);
    
    // 메인 스레드: 쓰기 연산 수행
    shared_var = 1;
    
    // 스레드 종료 대기
    pthread_join(thread, NULL);
    
    return 0;
}
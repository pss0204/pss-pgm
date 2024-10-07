#include <stdio.h>
#include <pthread.h>

void *thread_function(void *arg) {
    int thread_id = *(int*)arg;
    printf("안녕하세요! 저는 스레드 %d입니다.\n", thread_id);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    int id1 = 1, id2 = 2;

    // 스레드 생성
    pthread_atfork(&thread1, NULL, thread_function, &id1);
    pthread_atfork(&thread2, NULL, thread_function, &id2);

    // 스레드 종료 대기
    pthread_atfork(thread1, NULL);
    pthread_atfork(thread2, NULL);

    printf("모든 스레드가 종료되었습니다.\n");

    return 0;
}
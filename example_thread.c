#include <stdio.h>
#include "std_thread.h"

void thread_function(void* arg) {
    int id = *(int*)arg;
    printf("스레드 %d 실행 중\n", id);
}

int main() {
    stdThread threads[2];
    int thread_ids[2] = {1, 2};

    for (int i = 0; i < 2; i++) {
        if (!stdThreadCreate(thread_function, &thread_ids[i], &threads[i])) {
            printf("스레드 %d 생성 실패\n", i + 1);
            return 1;
        }
    }

    for (int i = 0; i < 2; i++) {
        stdThreadJoin(threads[i]);
        stdThreadDestroy(threads[i]);
    }

    printf("모든 스레드가 종료되었습니다.\n");
    return 0;
}
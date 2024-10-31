#include <stdio.h>
#include <pthread.h>

volatile int sync_point = 0;

// 스레드 함수는 void* 반환 타입이어야 합니다
void* thread1_func(void* arg) {
    asm volatile (
        "lui t0, 0x80\n\t"
        "addi t0, t0, 0x10\n\t"
        "lw t1, 0(t0)\n\t"
        ::: "t0", "t1"
    );
    
    sync_point = 1;
    asm volatile("fence" ::: "memory");
    
    asm volatile (
        "lui t0, 0x80\n\t"
        "addi t0, t0, 0x14\n\t"
        "lw t2, 0(t0)\n\t"
        "add t3, t1, t2\n\t"
        "sw t3, 0(t0)\n\t"
        ::: "t0", "t1", "t2", "t3"
    );
    
    return NULL;
}

void* thread2_func(void* arg) {
    while (sync_point != 1) {
        asm volatile("nop");
    }
    asm volatile("fence" ::: "memory");
    
    asm volatile (
        "lui t0, 0x80\n\t"
        "addi t0, t0, 0x10\n\t"
        "lw t1, 0(t0)\n\t"
        "addi t1, t1, 1\n\t"
        "sw t1, 0(t0)\n\t"
        ::: "t0", "t1"
    );
    
    return NULL;
}

int main(void) {
    pthread_t thread1, thread2;
    int ret1, ret2;

    // 스레드 생성
    ret1 = pthread_create(&thread1, NULL, thread1_func, NULL);
    if (ret1) {
        printf("Error creating thread 1: %d\n", ret1);
        return 1;
    }

    ret2 = pthread_create(&thread2, NULL, thread2_func, NULL);
    if (ret2) {
        printf("Error creating thread 2: %d\n", ret2);
        return 1;
    }

    // 스레드 종료 대기
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
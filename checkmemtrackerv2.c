#include <stdio.h>

int main() {
    unsigned long r5, r6, r7, r28, r29;
    unsigned int test_memory = 0xdeadbeef;

    // 레지스터 초기화 및 초기값 읽기
    asm volatile (
        "li x5, 0\n\t"
        "li x6, 0\n\t"
        "li x7, 0\n\t"
        "li x28, 0\n\t"
        "li x29, 0\n\t"
        "mv %0, x5\n\t"
        "mv %1, x6\n\t"
        "mv %2, x7\n\t"
        "mv %3, x28\n\t"
        "mv %4, x29"
        : "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r28), "=r" (r29)
        :
        : "x5", "x6", "x7", "x28", "x29"
    );

    // 초기화된 값 출력
    printf("초기화된 레지스터 값:\n");
    printf("Load or Store (x5): %lu\n", r5);
    printf("RS1 Addr (x6): 0x%lx\n", r6);
    printf("RS2 Addr (x7): 0x%lx\n", r7);
    printf("Hart ID (x28): %lu\n", r28);
    printf("Num Src (x29): %lu\n\n", r29);

    // 로드 명령어 실행 및 레지스터 값 읽기
    asm volatile (
        "lw x10, %5\n\t"
        "mv %0, x5\n\t"
        "mv %1, x6\n\t"
        "mv %2, x7\n\t"
        "mv %3, x28\n\t"
        "mv %4, x29"
        : "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r28), "=r" (r29)
        : "m" (test_memory)
        : "x10"
    );

    // 로드 후 결과 출력
    printf("로드 명령어 실행 후 레지스터 값:\n");
    printf("Load or Store (x5): %lu\n", r5);
    printf("RS1 Addr (x6): 0x%lx\n", r6);
    printf("RS2 Addr (x7): 0x%lx\n", r7);
    printf("Hart ID (x28): %lu\n", r28);
    printf("Num Src (x29): %lu\n", r29);

    return 0;
}
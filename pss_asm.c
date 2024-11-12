#include <stdio.h>
#include <pthread.h>

// 전역 영역으로 이동
static volatile int PARK = 0;

int main() {
    printf("Hello, World!\n");
    __asm__ __volatile__(
        "la t6, PARK\n\t"
        ".word 0b11111111110011101000111100001011   # PARK x28, x29, x30\n\t"
        "sw t0, 0(t6)\n\t"
        "lw t0, 0(t6)\n\t"
        "sw t0, 0(t6)\n\t"
        "lw t0, 0(t6)\n\t"
        "sw t0, 0(t6)\n\t"
        "lw t0, 0(t6)\n\t"
        "sw t0, 0(t6)\n\t"
        "sw t0, 0(t6)\n\t"  
        "lw t0, 0(t6)\n\t"
        "sw t0, 0(t6)\n\t"
        "lw t0, 0(t6)\n\t"
        "sw t0, 0(t6)\n\t"
        "lw t0, 0(t6)\n\t"
        ".word 0b11111111110011101000111100001011   # PARK x28, x29, x30\n\t"
        ".word 0b00000000011100101001001011111011    # TEST_RACE x6, x5, x7\n\t"
        :
        :
        : "memory"
    );
    return 0;
}

#include <stdio.h>
#include <pthread.h>

int main() {
    printf("Hello, World!\n");
    __asm__ __volatile__(
        ".word 0b11111110110001011000010100001011   # PARK x10, x11, x12\n\t"
        "lw t0, 0(sp)\n\t"
        "lw t1, 4(sp)\n\t"
        "add t2, t0, t1\n\t"
        "sw t2, 8(sp)\n\t"
        "lw t3, 12(sp)\n\t"
        "add t4, t2, t3\n\t"
        "sw t4, 16(sp)\n\t"
        "lw t5, 20(sp)\n\t"
        "add t6, t4, t5\n\t"
        "sw t6, 24(sp)\n\t"
        ".word 0b11111110110001011000010100001011   # PARK x10, x11, x12\n\t"
        ".word 0b00000000011100101001001011111011    # TEST_RACE x6, x5, x7\n\t"
        :
        :
        : "memory"
    );
    return 0;
}

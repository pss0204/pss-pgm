#include <stdio.h>
#include <pthread.h>

int main() {
    printf("Hello, World!\n");
    __asm__ __volatile__(
        ".word 0b11111111110011101000111100001011   # PARK x28, x29, x30\n\t"
       
        "sw t2, 0(sp)\n\t"
        "lw t0, 0(sp)\n\t"
        "sw t1, 0(sp)\n\t"
        "lw t0, 0(sp)\n\t"
        ".word 0b11111111110011101000111100001011   # PARK x28, x29, x30\n\t"
        ".word 0b00000000011100101001001011111011    # TEST_RACE x6, x5, x7\n\t"
        :
        :
        : "memory"
    );
    return 0;
}

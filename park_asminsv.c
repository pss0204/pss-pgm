#include <stdio.h>
#include <pthread.h>

int main() {
    printf("Hello, World!\n");
    __asm__ __volatile__(
        // PARK 명령어를 .insn 지시어로 사용
        ".insn r 0x0B, 0, x10, x11, x12, 0x7F   # PARK x10, x11, x12\n\t"
        
        "lw t0, 0(sp)\n\t"          // 스택에서 값을 로드
        "lw t1, 4(sp)\n\t"          // 다음 값 로드
        "add t2, t0, t1\n\t"        // t0와 t1을 더해서 t2에 저장
        "sw t2, 8(sp)\n\t"          // 결과를 스택에 저장
        "lw t3, 12(sp)\n\t"         // 새로운 값 로드
        "add t4, t2, t3\n\t"        // t2와 t3을 더함
        "sw t4, 16(sp)\n\t"         // 결과 저장
        "lw t5, 20(sp)\n\t"         // 다시 로드
        "add t6, t4, t5\n\t"        // 더하기 연산
        "sw t6, 24(sp)\n\t"         // 최종 결과 저장
        
        // 다시 PARK 명령어 사용
        ".insn r 0x0B, 0, x10, x11, x12, 0x7F   # PARK x10, x11, x12\n\t"
        
        // TEST_RACE 명령어를 .insn 지시어로 사용
        ".insn r 0x7B, 1, x6, x5, x7, 0x00   # TEST_RACE x6, x5, x7\n\t"
        :
        :
        : "memory"
    );
    return 0;
}

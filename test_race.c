#include <stdio.h>

int main() {
    // TEST_RACE 명령어 수행을 위한 레지스터 설정
    register int x5 asm ("x5") = 10;
    register int x6 asm ("x6") = 20;
    register int x7 asm ("x7") = 30;
    register int x28 asm ("x28") = 0;
    printf("initial x28: %d",x28);
    // TEST_RACE 명령어 사용
    asm volatile (
        ".word 0b00000000011100101001001011111011    # TEST_RACE x6, x5, x7"
        : "+r"(x6)
        : "r"(x5), "r"(x7)
    );

    // 레지스터 값 출력
    printf("x5: %d, x6: %d, x7: %d\n", x5, x6, x7);
    printf("after test_race x28: %d\n", x28);
    return 0;
}

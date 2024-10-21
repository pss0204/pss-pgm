#include <stdio.h>

int main() {
    unsigned long r5, r6, r7;

    // TEST_RACE 명령어 실행 및 레지스터 값 읽기
    asm volatile (
        ".word 0x0015B5B3\n\t"  // TEST_RACE 명령어 (0001011 00101 11011 001 00101 1111011)
        "mv %0, x5\n\t"         // x5 (r5) 값을 r5 변수로 이동
        "mv %1, x6\n\t"         // x6 (r6) 값을 r6 변수로 이동
        "mv %2, x7"             // x7 (r7) 값을 r7 변수로 이동
        : "=r" (r5), "=r" (r6), "=r" (r7)  // 출력 오퍼랜드
        :                                  // 입력 오퍼랜드 (없음)
        : "x5", "x6", "x7"                 // 클로버드 레지스터
    );

    // 결과 출력
    printf("r5 (x5) 값: 0x%lx\n", r5);
    printf("r6 (x6) 값: 0x%lx\n", r6);
    printf("r7 (x7) 값: 0x%lx\n", r7);

    return 0;
}
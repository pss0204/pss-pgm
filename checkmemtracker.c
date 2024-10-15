#include <stdint.h>
#include <stdio.h>

// 레지스터 값을 읽는 인라인 함수
static inline uint32_t read_register(int reg_num) {
    uint32_t value;
    asm volatile ("mv %0, x%1" : "=r"(value) : "i"(reg_num));
    return value;
}

// 레지스터 값을 쓰는 인라인 함수
static inline void write_register(int reg_num, uint32_t value) {
    asm volatile ("mv x%0, %1" : : "i"(reg_num), "r"(value));
}

// 로드 명령어 실행 함수
void execute_load(uint32_t* addr) {
    uint32_t value;
    asm volatile ("lw %0, 0(%1)" : "=r"(value) : "r"(addr));
}

int main() {
    // 테스트용 메모리 주소
    uint32_t test_memory[1] = {0xdeadbeef};

    // 관련 레지스터 초기화
    write_register(5, 0x11111111);  // RS1 Addr
    write_register(6, 0x22222222);  // RS2 Addr
    write_register(7, 0x33333333);  // Hart ID
    write_register(28, 0);          // Is Load
    write_register(29, 0);          // Is Store

    printf("초기화된 레지스터 값:\n");
    printf("RS1 Addr (x5): 0x%08x\n", read_register(5));
    printf("RS2 Addr (x6): 0x%08x\n", read_register(6));
    printf("Hart ID (x7): 0x%08x\n", read_register(7));
    printf("Is Load (x28): %u\n", read_register(28));
    printf("Is Store (x29): %u\n\n", read_register(29));

    // 로드 명령어 실행
    execute_load(test_memory);

    printf("로드 명령어 실행 후 레지스터 값:\n");
    printf("RS1 Addr (x5): 0x%08x\n", read_register(5));
    printf("RS2 Addr (x6): 0x%08x\n", read_register(6));
    printf("Hart ID (x7): 0x%08x\n", read_register(7));
    printf("Is Load (x28): %u\n", read_register(28));
    printf("Is Store (x29): %u\n", read_register(29));

    return 0;
}

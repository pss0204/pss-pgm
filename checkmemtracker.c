#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// MMIO 주소 정의
#define MMIO_ACCESS_HISTORY_BASE 0x20000000
#define MMIO_ACCESS_HISTORY_SIZE 10
#define ENTRY_SIZE_BYTES 32

typedef struct {
    uint32_t pc;         // 0~3 바이트
    uint32_t rs1_addr;   // 4~7 바이트
    uint32_t rs2_addr;   // 8~11 바이트
    uint32_t flags;      // 12~15 바이트 (isLoad, isStore, 나머지 30비트는 0)
    uint32_t hartid;     // 16~19 바이트
    uint32_t num_src;    // 20~23 바이트
    uint32_t reserved[2];// 24~31 바이트 (예약)
} MemoryAccess;

int main() {
    // MMIO 주소를 메모리로 매핑 (가상 예시)
    volatile MemoryAccess* accessHistory = (MemoryAccess*)MMIO_ACCESS_HISTORY_BASE;

    for(int i = 0; i < MMIO_ACCESS_HISTORY_SIZE; i++) {
        MemoryAccess entry = accessHistory[i];

        // flags에서 isLoad과 isStore 추출
        bool isLoad = (entry.flags & 0x1) != 0;
        bool isStore = (entry.flags & 0x2) != 0;

        printf("Access History %d:\n", i);
        printf("  PC: 0x%08x\n", entry.pc);
        printf("  RS1 Addr: 0x%08x\n", entry.rs1_addr);
        printf("  RS2 Addr: 0x%08x\n", entry.rs2_addr);
        printf("  Is Load: %d\n", isLoad);
        printf("  Is Store: %d\n", isStore);
        printf("  Hart ID: %d\n", entry.hartid);
        printf("  Num Src: %d\n\n", entry.num_src);
    }

    return 0;
}
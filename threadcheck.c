#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>

pid_t gettid(void) {
    return syscall(SYS_gettid);
}

void process_function(const char* process_name) {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t tid = gettid();

    printf("%s:\n", process_name);
    printf("  프로세스 ID (PID): %d\n", pid);
    printf("  부모 프로세스 ID (PPID): %d\n", ppid);
    printf("  쓰레드 ID (TID): %d\n", tid);
    printf("  프로세스 그룹 ID (PGID): %d\n\n", getpgid(0));
}

int main() {
    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 == 0) {
        // 첫 번째 자식 프로세스
        process_function("자식 프로세스 1");
        exit(0);
    } else if (pid1 < 0) {
        perror("fork 실패");
        exit(1);
    }

    pid2 = fork();
    if (pid2 == 0) {
        // 두 번째 자식 프로세스
        process_function("자식 프로세스 2");
        exit(0);
    } else if (pid2 < 0) {
        perror("fork 실패");
        exit(1);
    }

    // 부모 프로세스
    process_function("부모 프로세스");

    // 자식 프로세스들이 종료될 때까지 대기
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
/* 통합된 cwe366.c 파일 */

/* 표준 라이브러리 포함 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h> /* open/close 등 */
#include <process.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

/* 매크로 정의 */
#define N_ITERS 1000000
#define INCLUDEMAIN

/* 전역 변수 정의 */
static int gBadInt = 0;
static int gGoodInt = 0;
static struct _stdThreadLock *gGoodLock = NULL;

/* 구조체 및 타입 정의 */
typedef struct _twoIntsStruct
{
    int intOne;
    int intTwo;
} twoIntsStruct;

typedef struct _stdThread *stdThread;
typedef struct _stdThreadLock *stdThreadLock;
typedef void (*stdThreadRoutine)(void *args);

/* stdThread 구조체 정의 */
struct _stdThread {
#ifdef _WIN32
    uintptr_t handle;
#else
    pthread_t handle;
#endif
    stdThreadRoutine start;
    void *args;
};

/* stdThreadLock 구조체 정의 */
struct _stdThreadLock {
#ifdef _WIN32
    CRITICAL_SECTION CriticalSection;
#else
    pthread_mutex_t mutex;
#endif
};

/* 함수 선언 */
int stdThreadCreate(stdThreadRoutine start, void *args, stdThread *thread);
int stdThreadJoin(stdThread thread);
int stdThreadDestroy(stdThread thread);

int stdThreadLockCreate(stdThreadLock *lock);
void stdThreadLockAcquire(stdThreadLock lock);
void stdThreadLockRelease(stdThreadLock lock);
void stdThreadLockDestroy(stdThreadLock lock);

/* Print 함수 구현 */
void printLine(const char * line) {
    printf("%s\n", line);
}

void printIntLine(int intNumber) {
    printf("%d\n", intNumber);
}

/* stdThread 함수 구현 */
#ifdef _WIN32
static unsigned __stdcall internal_start(void *args)
#else
static void *internal_start(void *args)
#endif
{
    stdThread thread = (stdThread)args;

    thread->start(thread->args);

#ifdef _WIN32
    _endthreadex(0);
    /* 경고 방지를 위해 반환 */
    return 0;
#else
    pthread_exit(NULL);
    /* 경고 방지를 위해 반환 */
    return NULL;
#endif
}

int stdThreadCreate(stdThreadRoutine start, void *args, stdThread *thread)
{
#ifdef _WIN32
    uintptr_t handle;
#else
    pthread_t handle;
#endif
    stdThread my_thread;

    *thread = NULL;

    my_thread = (stdThread)malloc(sizeof(*my_thread));
    if (my_thread == NULL) {
        return 0;
    }

    my_thread->start = start;
    my_thread->args = args;

#ifdef _WIN32
    handle = _beginthreadex(NULL, 0, internal_start, my_thread, 0, NULL);
    if (handle == 0) {
        free(my_thread);
        return 0;
    }
#else
    if (0 != pthread_create(&handle, NULL, internal_start, my_thread)) {
        free(my_thread);
        return 0;
    }
#endif

    /* 스레드 핸들 초기화 */
    my_thread->handle = handle;

    *thread = my_thread;

    return 1;
}

int stdThreadJoin(stdThread thread)
{
#ifdef _WIN32
    DWORD value;

    value = WaitForSingleObject((HANDLE)thread->handle, INFINITE);
    if (value != WAIT_OBJECT_0) return 0;
#else
    void *dummy;
    if (0 != pthread_join(thread->handle, &dummy)) return 0;
#endif

    return 1;
}

int stdThreadDestroy(stdThread thread)
{
#ifdef _WIN32
    CloseHandle((HANDLE)thread->handle);
#endif
    free(thread);

    return 1;
}

/* stdThreadLock 함수 구현 */
int stdThreadLockCreate(stdThreadLock *lock)
{
    stdThreadLock my_lock = NULL;

    *lock = NULL;

    my_lock = (stdThreadLock)malloc(sizeof(*my_lock));
    if (my_lock == NULL) return 0;

#ifdef _WIN32
    __try {
        InitializeCriticalSection(&my_lock->CriticalSection);
    } __except (GetExceptionCode() == STATUS_NO_MEMORY) {
        free(my_lock);
        return 0;
    }
#else
    if (0 != pthread_mutex_init(&my_lock->mutex, NULL)) {
        free(my_lock);
        return 0;
    }
#endif

    *lock = my_lock;

    return 1;
}

void stdThreadLockAcquire(stdThreadLock lock)
{
#ifdef _WIN32
    /* Windows 크리티컬 섹션 입장 */
    EnterCriticalSection(&lock->CriticalSection);
#else
    pthread_mutex_lock(&lock->mutex);
#endif
}

void stdThreadLockRelease(stdThreadLock lock)
{
#ifdef _WIN32
    /* Windows 크리티컬 섹션 해제 */
    LeaveCriticalSection(&lock->CriticalSection);
#else
    pthread_mutex_unlock(&lock->mutex);
#endif
}

void stdThreadLockDestroy(stdThreadLock lock)
{
#ifdef _WIN32
    DeleteCriticalSection(&lock->CriticalSection);
#else
    pthread_mutex_destroy(&lock->mutex);
#endif
    free(lock);
}

/* CWE366 관련 함수 구현 */
static void helperBad(void *args)
{
    int i;
    /* 취약점: 전역 변수 증가가 원자적으로 이루어지지 않음 */
    for (i = 0; i < N_ITERS; i++)
    {
        gBadInt = gBadInt + 1;
    }
}

static void helperGood(void *args)
{
    int i;
    /* 수정: 락을 획득하여 원자적으로 전역 변수 증가 */
    stdThreadLockAcquire(gGoodLock);
    for (i = 0; i < N_ITERS; i++)
    {
        gGoodInt = gGoodInt + 1;
    }
    stdThreadLockRelease(gGoodLock);
}

#ifndef OMITBAD

void CWE366_Race_Condition_Within_Thread__global_int_01_bad()
{
    stdThread threadA = NULL;
    stdThread threadB = NULL;
    gBadInt = 0;

    if (!stdThreadCreate(helperBad, NULL, &threadA))
    {
        threadA = NULL;
    }
    if (!stdThreadCreate(helperBad, NULL, &threadB))
    {
        threadB = NULL;
    }
    if (threadA && stdThreadJoin(threadA))
    {
        stdThreadDestroy(threadA);
    }
    if (threadB && stdThreadJoin(threadB))
    {
        stdThreadDestroy(threadB);
    }
    printIntLine(gBadInt);
}

#endif /* OMITBAD */

#ifndef OMITGOOD

static void good1()
{
    stdThread threadA = NULL;
    stdThread threadB = NULL;

    if (!stdThreadLockCreate(&gGoodLock))
    {
        return;
    }
    if (!stdThreadCreate(helperGood, NULL, &threadA))
    {
        threadA = NULL;
    }
    if (!stdThreadCreate(helperGood, NULL, &threadB))
    {
        threadB = NULL;
    }
    if (threadA && stdThreadJoin(threadA))
    {
        stdThreadDestroy(threadA);
    }
    if (threadB && stdThreadJoin(threadB))
    {
        stdThreadDestroy(threadB);
    }
    stdThreadLockDestroy(gGoodLock);
    printIntLine(gGoodInt);
}

void CWE366_Race_Condition_Within_Thread__global_int_01_good()
{
    good1();
}

#endif /* OMITGOOD */

/* 메인 함수 구현 */
#ifdef INCLUDEMAIN

int main(int argc, char * argv[])
{
    /* 랜덤 시드 초기화 */
    srand( (unsigned)time(NULL) );

#ifndef OMITGOOD
    printLine("Calling good()...");
    CWE366_Race_Condition_Within_Thread__global_int_01_good();
    printLine("Finished good()");
#endif /* OMITGOOD */
#ifndef OMITBAD
    printLine("Calling bad()...");
    CWE366_Race_Condition_Within_Thread__global_int_01_bad();
    printLine("Finished bad()");
#endif /* OMITBAD */
    return 0;
}

#endif
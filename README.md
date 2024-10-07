# 쓰레드 아이디 하드웨어에 기록

```scala
//csr.scala 파일 
class CSRFileIO(implicit p: Parameters) extends CoreBundle
    with HasCoreParameters {
  val ungated_clock = Clock().asInput
  val interrupts = new CoreInterrupts().asInput
  val hartid = UInt(INPUT, hartIdLen)// 여기에 매핑 
  val rw = new Bundle {
    val addr = UInt(INPUT, CSR.ADDRSZ)
    val cmd = Bits(INPUT, CSR.SZ)
    val rdata = Bits(OUTPUT, xLen)
    val wdata = Bits(INPUT, xLen)
  }
```

- hartid 라는 레지스터에 소프트웨어에서 매핑을 해줌(어캐하는진 모름)

```scala
//csr.scala
class MemoryAccessTracker(implicit p: Parameters) extends CoreModule()(p) {
  val io = IO(new Bundle {
    val pc = UInt(INPUT, vaddrBitsExtended)
    val addr = UInt(INPUT, vaddrBitsExtended)
    val isLoad = Bool(INPUT)
    val isStore = Bool(INPUT)
    val hartid = UInt(INPUT, hartIdLen)
  })

  class MemoryAccess extends Bundle {
    val pc = UInt(width = vaddrBitsExtended)
    val addr = UInt(width = vaddrBitsExtended)
    val isLoad = Bool()
    val isStore = Bool()
    val hartid = UInt(width = hartIdLen)
  }

  val accessHistory = Reg(Vec(10, new MemoryAccess))
  val accessCounter = Reg(init = UInt(0, log2Ceil(10)))

  when (io.isLoad || io.isStore) {
    val newAccess = Wire(new MemoryAccess)
    newAccess.pc := io.pc
    newAccess.addr := io.addr
    newAccess.isLoad := io.isLoad
    newAccess.isStore := io.isStore
    newAccess.hartid := io.hartid

    accessHistory(accessCounter) := newAccess
    accessCounter := Mux(accessCounter === UInt(9), UInt(0), accessCounter + UInt(1))
  }
}
```

```scala
//Rocketcore.scala

  val memoryTracker = Module(new MemoryAccessTracker())
  memoryTracker.io.pc := wb_reg_pc // wb_reg_pc는 변경 없음
  memoryTracker.io.addr := wb_reg_rs2 // wb_reg_rs2는 변경 없음
  memoryTracker.io.isLoad := io.dmem.req.bits.cmd === M_XRD // dcache에서 io.dmem으로 변경
  memoryTracker.io.isStore := io.dmem.req.bits.cmd === M_XWR // dcache에서 io.dmem으로 변경
  memoryTracker.io.hartid := io.hartid // io.hartid는 변경 없음
```

![pid 는 pgm 당 하나인것 같고 쓰레드 아이디는 여러개 가능 ](https://prod-files-secure.s3.us-west-2.amazonaws.com/02b59d1c-e1e9-496e-8f0b-7c08603ed7c2/02350280-56d2-458d-935f-459ced3ca26d/image.png)

pid 는 pgm 당 하나인것 같고 쓰레드 아이디는 여러개 가능 

```c
//parkthread.c
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *p_function(void * data)
{
  pid_t pid; //process id
  pthread_t tid; // thread id

  pid = getpid(); //4
  tid = pthread_self();

  char* thread_name = (char *)data;
  int i = 0;

  while(i<3)
  {
    printf("thread name : %s, tid : %x, pid : %u\n", thread_name, (unsigned int)tid, (unsigned int)pid); //5
    i++;
    sleep(1);
  }
}

int main(void)
{
  pthread_t pthread[2];
  int thr_id;
  int status;
  char p1[] = "thread_1";
  char p2[] = "thread_2";
  char p3[] = "thread_3";

  sleep(1); //1

  thr_id = pthread_create(&pthread[0], NULL, p_function, (void*)p1); //2
  if(thr_id < 0)
  {
    perror("pthread0 create error");
    exit(EXIT_FAILURE);
  }

  thr_id = pthread_create(&pthread[1], NULL, p_function, (void *)p2); //2
  if(thr_id < 0)
  {
    perror("pthread1 create error");
    exit(EXIT_FAILURE);
  }

  p_function((void *)p3); //3

  pthread_join(pthread[0], (void **)&status); //6
  pthread_join(pthread[1], (void **)&status);

  printf("end??\n");
  return 0;
}
```

# 쓰레드 아이디 분석

- tgid=pid
- 프로그램 짜서 임상적으로 확인 했을때는 부모-자식간에 각각 별도의 pid 를 가지는것으로 확인
- 

![image.png](https://prod-files-secure.s3.us-west-2.amazonaws.com/02b59d1c-e1e9-496e-8f0b-7c08603ed7c2/363f4614-5e7e-4e86-9e4f-4358306b96d2/image.png)

```c
//이쁘게 정리한거
1. 부모 프로세스:
프로세스 ID (PID): 110
부모 프로세스 ID (PPID): 106 (이 프로세스를 실행한 셸의 PID일 것입니다)
쓰레드 ID (TID): 110 (단일 쓰레드 프로세스에서 PID와 TID는 동일합니다)
프로세스 그룹 ID (PGID): 110 (기본적으로 프로세스 그룹 리더의 PID와 같습니다)

자식 프로세스 1:
프로세스 ID (PID): 111
부모 프로세스 ID (PPID): 110 (부모 프로세스의 PID)
쓰레드 ID (TID): 111
프로세스 그룹 ID (PGID): 110 (부모로부터 상속받은 PGID)

자식 프로세스 2:
프로세스 ID (PID): 112
부모 프로세스 ID (PPID): 110 (부모 프로세스의 PID)
쓰레드 ID (TID): 112
프로세스 그룹 ID (PGID): 110 (부모로부터 상속받은 PGID)
```

```c
//threadcheck.c
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
```

- 부모자식 안쓰고 그냥 멀티 쓰레드 쓰면 똑같은 tgid 임
- 

![image.png](https://prod-files-secure.s3.us-west-2.amazonaws.com/02b59d1c-e1e9-496e-8f0b-7c08603ed7c2/b42955db-e8d4-4a94-8363-4cf0e6b04335/image.png)

- 부모자식으로 트랩 잡으니까 첫 쓰레드에 바로  잡히는데

![image.png](https://prod-files-secure.s3.us-west-2.amazonaws.com/02b59d1c-e1e9-496e-8f0b-7c08603ed7c2/3f893468-3af3-4fe3-a788-5274bb6aa423/image.png)

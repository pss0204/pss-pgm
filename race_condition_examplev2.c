#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
//버전2는 부모랑 자식 프로세스 분리함 
#define NUM_process 2



void test_race(){

    register int x5 asm ("x5") = 1;
    register int x6 asm ("x6") = 2;
    register int x7 asm ("x7") = 3;

     asm volatile (
        ".word 0b00000000011100101001001011111011    # TEST_RACE x6, x5, x7"
        : "+r"(x6)
        : "r"(x5), "r"(x7)
    );

}


int main(){
    pid_t pids[NUM_process];
    for(int i = 0; i < NUM_process; i++){
        pids[i]=fork();
        if(pids[i] == 0){
            //son process
            test_race();
            return 0;
        }else if (pids[i] < 0){
            perror("fork failed");
            return 1;
        }
    }

    for(int i = 0; i < NUM_process; i++){
        waitpid(pids[i], NULL, 0);
    }

printf("wait end\n");
return 0;
}

    
    


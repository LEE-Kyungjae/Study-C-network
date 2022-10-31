/*  조건 다음의 시스템 콜 함수를 사용할것
fork() 자식프로세스 작성
execv() 프로세스가 자기자신을 "지정한 프로그램"으로 바뀐다
wait() 자식프로세스가 종료할때까지 대기. 자식프로세스의 종료 스테이터스를 수신받음
exit() 프로그램을 종료한다. 인수는 종료스테이터스로써 부모프로세스에

*/
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
int main()
{ //선언
    //pid_t fork(void);
    //pid_t wait(int *status);
    //void exit(int status);
    //int execv(const char *path, char *const argv[]);
    pid_t pid;
    int res;
    char *argv[] = {"/bin/ls",  NULL};
    char *argv2[] = {"/bin/exit",NULL};
    char *argv3[] = {"/bin/quit",NULL};
    //"-a", "-l",
    //코딩
    pid = fork();
    if (pid == 0)
    { //자식프로세스실행

   errno = 0;
   res = execv(argv[0], argv);
   if (errno != 0)
    printf("Command not found.\n");
      _exit(0); /* exit() を使わない点に注意 */

    }
    else if (pid > 0)
    { //부모프로세스 실행 pid=1
      wait(&res);
      printf("Parent exiting...\n");  
      exit(0);
    }
    else
    { //에러처리
        fprintf(stderr, "can't fork, error %d\n", errno);
        exit(EXIT_FAILURE);
    }
    //리턴
    return 0;
}
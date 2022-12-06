#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
// 입력 "ls -al &"
// ==> tokens:[ls","-al","&",NULL] , t_count: 3
int tokenize(char *buf, char *tokens[]) // 입력문자열을 공백단위로 자르기
{
    int t_count = 0;
    char *token;
    token = strtok(buf, " \n");
    // printf("%s\n",token);
    while (token != NULL)
    {
        tokens[t_count++] = token;
        token = strtok(NULL, " \n");
        // printf("%s\n",token);
    }
    tokens[t_count] = NULL;
    return t_count;
}
// "ls","-al","&", NULL ,t_count: 3
// ==> "ls","-al", NULL
int isbackground(char *tokens[], int token_count)
{
    if (strcmp(tokens[token_count - 1], "&") == 0)
    {
        tokens[token_count - 1] = NULL; // &를 지움
        return 1;
    }
    return 0;
}
// pathlist
// /usr/local/sbin
// /usr/local/bin
// /usr/sbin
// /usr/bin
// /sbin
// /bin
// token : ls
//
// ==> cmd /bin/ls
int getcmd(char *pathlist[], int count, char token[], char cmd[])
{
    for (int i = count - 1; i > 0; i--)
    {
        sprintf(cmd, "%s/%s", pathlist[i], token); // /bin/ls

        if (access(cmd, F_OK) == 0)
        {
            // file exists
            printf("cmd %s\n", cmd);
            return 1;
        }
    }
    return 0;
}
int run(char *pathlist[], int count, char *line)
{
    pid_t pid;
    int i, j, fd, token_count;
    int status;
    int backgroundflag = 0;
    int redirectidx = 0;
    char *tokens[10];
    token_count = tokenize(line, tokens);
    if (token_count == 0)
        return 1;
    /*
     printf("inputcommand[%s] : ", tokens[0]);
    printf("argument");
    for (i = 1; i < token_count; i++)
    {
    printf("[%s] ", tokens[i]);
    }
    printf("\n");
    */
    backgroundflag = isbackground(tokens, token_count);
    if (backgroundflag)
        token_count--;
    if (strcmp(tokens[0], "exit") == 0)
    {
        return 0;
    }
    char cmd[256];
    if (getcmd(pathlist, count, tokens[0], cmd) == 0)
    {
        fprintf(stderr, "%s Command not found.\n", tokens[0]);
        fflush(stderr);
        return 1;
    }

    if ((pid = fork()) == 0) // child /bin/ls
    {
        execv(cmd, tokens);
    }
    else if (pid > 0)
    {
        if (!backgroundflag)
        {
            wait(&status);
            // printf("pid %d\n",pid);
        }
    }
    return 1;
}
void siginthandler()
{
    printf("siginthandler\n");
}
void sigchildhandler()
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (WIFEXITED(status) != 0)
        {
            printf("Done [%d]\n", pid);
        }
    }
}
int getpathlist(char *pathlist[100])
{
    char env[4000];
    int count = 0;
    strcpy(env, getenv("PATH"));
    char *tok = strtok(env, ":\r\n");
    while (tok)
    {
        // printf("%s\n",tok);
        pathlist[count] = tok;
        tok = strtok(NULL, ":\r\n");
        count++;
    }
    return count;
}
int main()
{
    char line[1024];
    char *pathlist[100];

    int count = getpathlist(pathlist);

    signal(SIGINT, siginthandler);
    signal(SIGCHLD, sigchildhandler);

    while (1)
    {
        printf("> ");
        fgets(line, sizeof(line) - 1, stdin);
        if (run(pathlist, count, line) == 0)
            break;
    }
    printf("Bye!!!\n");
    return 0;
}
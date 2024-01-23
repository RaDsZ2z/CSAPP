#include "csapp.h"
using namespace std;
const int MAXARGS = 128;
const int MAXLINE = 128;

void eval(char *cmdline, char *envp[]);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

// 对系统调用进行包装
void unix_error(const char *msg);
pid_t Fork();
void Fgets(char *str);

int main(int argc, char *argv[], char *envp[])
{
    char cmdline[MAXLINE];
    while (1)
    {
        printf("> ");
        Fgets(cmdline);
        eval(cmdline, envp);
    }
}
void eval(char *cmdline, char *envp[])
{
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == nullptr)
        return;
    if (!builtin_command(argv))
    {
        if ((pid = Fork()) == 0)
        {
            //-----------
            // printf("argv:\n");
            // for (int i = 0; argv[i] != nullptr; i++)
            // {
            //     printf("argv[%d]:%s\n", i, argv[i]);
            // }
            // printf("envp:\n");
            // for (int i = 0; envp[i] != nullptr; i++)
            // {
            //     printf("envp[%d]:%s\n", i, envp[i]);
            // }
            //-----------
            if (execve(argv[0], argv, envp) < 0)
            {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        if (!bg)
        {
            int status;
            if (waitpid(pid, &status, 0) < 0)
                unix_error("waitfg: waitpid error");
        }
        else
            printf("%d %s", pid, cmdline);
    }
    return;
}
int builtin_command(char **argv)
{
    if (!strcmp(argv[0], "quit"))
        exit(0);
    if (!strcmp(argv[0], "&"))
        return 1;
    return 0;
}
int parseline(char *buf, char **argv)
{
    char *delim;
    int argc;
    int bg;

    buf[strlen(buf) - 1] = ' ';
    while (*buf && (*buf == ' '))
        buf++;
    /* Build the argc list*/
    argc = 0;
    while ((delim = strchr(buf, ' ')))
    {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' '))
            buf++;
    }
    argv[argc] = nullptr;

    if (argc == 0)
        return 1;
    /* Should the job run in the background? */
    if ((bg = (*argv[argc - 1] == '&')) != false)
        argv[--argc] = nullptr;

    return bg;
}

pid_t Fork()
{
    pid_t pid;
    if ((pid = fork()) < 0)
        unix_error("Fork error");
    return pid;
}

void Fgets(char *str)
{
    fgets(str, MAXLINE, stdin);
    int len = strlen(str);
    if (len >= MAXLINE - 1)
        unix_error("Fgets error");
}

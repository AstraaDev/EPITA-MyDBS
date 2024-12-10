#include "my_db.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_register(struct user_regs_struct regs)
{
    printf("rax %lld", regs.rax);
    printf("rbx %lld", regs.rbx);
    printf("rcx %lld", regs.rcx);
    printf("rdx %lld", regs.rdx);
    printf("rsi %lld", regs.rsi);
    printf("rdi %lld", regs.rdi);
    printf("rbp %lld", regs.rbp);
    printf("rsp %lld", regs.rsp);
    printf("r8 %lld", regs.r8);
    printf("r9 %lld", regs.r9);
    printf("r10 %lld", regs.r10);
    printf("r11 %lld", regs.r11);
    printf("r12 %lld", regs.r12);
    printf("r13 %lld", regs.r13);
    printf("r14 %lld", regs.r14);
    printf("r15 %lld", regs.r15);
    printf("rip %lld", regs.rip);
    printf("eflags %lld", regs.eflags);
    printf("cs %lld", regs.cs);
    printf("ss %lld", regs.ss);
    printf("ds %lld", regs.ds);
    printf("es %lld", regs.es);
    printf("fs %lld", regs.fs);
    printf("gs %lld", regs.gs);
    printf("orig_rax %lld", regs.orig_rax);
}

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 2)
    {
        fprintf(stderr, "my_db: Usage: ./my_db <file_path>\n");
        return 1;
    }

    int pid = fork();
    switch (pid)
    {
    case -1:
        fprintf(stderr, "my_db: Error: Fork failed");
        return 2;
    case 0:
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execve(argv[1], argv + 1, envp);
    default:
        break;
    }

    int status = 0;
    struct user_regs_struct regs = { 0 };

    char user_input[4096] = { 0 };

    waitpid(pid, &status, 0);

    while (1)
    {
    start:
        scanf("%s", user_input);

        if (strcmp(user_input, "continue"))
        {
            break;
        }
        else if (strcmp(user_input, "quit"))
        {
            return 0;
        }
        else if (strcmp(user_input, "quit"))
        {
            // kill(pid, 0);
            printf("kill\n"); // TODO
        }
        else if (strcmp(user_input, "register"))
        {
            print_register(regs);
        }
        else
        {
            printf("error");
            goto start;
        }

        while (1)
        {
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
            waitpid(pid, &status, 0);

            if (WIFEXITED(status))
                break;

            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
        }
    }

    fprintf(stderr, "program exited with code %d\n", WEXITSTATUS(status));
    return 0;
}

#include "my_db.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_register(struct user_regs_struct regs)
{
    printf("rax %lld\n", regs.rax);
    printf("rbx %lld\n", regs.rbx);
    printf("rcx %lld\n", regs.rcx);
    printf("rdx %lld\n", regs.rdx);
    printf("rsi %lld\n", regs.rsi);
    printf("rdi %lld\n", regs.rdi);
    printf("rbp %lld\n", regs.rbp);
    printf("rsp %lld\n", regs.rsp);
    printf("r8 %lld\n", regs.r8);
    printf("r9 %lld\n", regs.r9);
    printf("r10 %lld\n", regs.r10);
    printf("r11 %lld\n", regs.r11);
    printf("r12 %lld\n", regs.r12);
    printf("r13 %lld\n", regs.r13);
    printf("r14 %lld\n", regs.r14);
    printf("r15 %lld\n", regs.r15);
    printf("rip %lld\n", regs.rip);
    printf("eflags %lld\n", regs.eflags);
    printf("cs %lld\n", regs.cs);
    printf("ss %lld\n", regs.ss);
    printf("ds %lld\n", regs.ds);
    printf("es %lld\n", regs.es);
    printf("fs %lld\n", regs.fs);
    printf("gs %lld\n", regs.gs);
    printf("orig_rax %lld\n", regs.orig_rax);
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
    long syscall_number;

    char user_input[4096] = { 0 };

    waitpid(pid, &status, 0);

    while (1)
    {
    start:
        scanf("%s", user_input);

        if (!strcmp(user_input, "continue"))
        {
            ;
        }
        else if (!strcmp(user_input, "quit"))
        {
            ptrace(PTRACE_KILL, pid, NULL, NULL);
            return 0;
        }
        else if (!strcmp(user_input, "kill"))
        {
            ptrace(PTRACE_KILL, pid, NULL, NULL);
            break;
        }
        else if (!strcmp(user_input, "register"))
        {
            print_register(regs);
        }
        else
        {
            printf("Bad argument !!!!\n");
            goto start;
        }

        while (1)
        {
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
            waitpid(pid, &status, 0);

            syscall_number = regs.orig_rax;

            if (WIFEXITED(status))
                break;

            if (syscall_number == -1)
                break;

            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
        }
    }

    fprintf(stderr, "program exited with code %d\n", WEXITSTATUS(status));
    return 0;
}

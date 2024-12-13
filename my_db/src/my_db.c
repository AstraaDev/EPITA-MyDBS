#include "my_db.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void print_register(struct user_regs_struct regs)
{
    printf("rax 0x%llx\n", regs.rax);
    printf("rbx 0x%llx\n", regs.rbx);
    printf("rcx 0x%llx\n", regs.rcx);
    printf("rdx 0x%llx\n", regs.rdx);
    printf("rsi 0x%llx\n", regs.rsi);
    printf("rdi 0x%llx\n", regs.rdi);
    printf("rbp 0x%llx\n", regs.rbp);
    printf("rsp 0x%llx\n", regs.rsp);
    printf("r8 0x%llx\n", regs.r8);
    printf("r9 0x%llx\n", regs.r9);
    printf("r10 0x%llx\n", regs.r10);
    printf("r11 0x%llx\n", regs.r11);
    printf("r12 0x%llx\n", regs.r12);
    printf("r13 0x%llx\n", regs.r13);
    printf("r14 0x%llx\n", regs.r14);
    printf("r15 0x%llx\n", regs.r15);
    printf("rip 0x%llx\n", regs.rip);
    printf("eflags 0x%llx\n", regs.eflags);
    printf("cs 0x%llx\n", regs.cs);
    printf("ss 0x%llx\n", regs.ss);
    printf("ds 0x%llx\n", regs.ds);
    printf("es 0x%llx\n", regs.es);
    printf("fs 0x%llx\n", regs.fs);
    printf("gs 0x%llx\n", regs.gs);
    printf("orig_rax 0x%llx\n", regs.orig_rax);
}

void print_memdump(int flag, int count, void *ptr, int pid)
{
    (void)flag;
    char *ptrTmp = ptr;
    long long val;
    for (int i = 0; i < count; i++)
    {
        val = ptrace(PTRACE_PEEKDATA, pid, ptrTmp, NULL);

        printf("%p 0x%llx\n", ptrTmp++, val);
    }
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

    char **input_parse = NULL;
    int nbArg = 0;

    while (1)
    {
        while (1)
        {
            scanf("%[^\n]%*c", user_input); // little bug here without input
            if (input_parse)
            {
                free_parse(input_parse);
                nbArg = 0;
            }

            input_parse = parser(user_input, &nbArg);

            if (input_parse == NULL)
            {
                continue;
            }
            else if (!strcmp(input_parse[0], "continue"))
            {
                break;
            }
            else if (!strcmp(input_parse[0], "quit"))
            {
                ptrace(PTRACE_KILL, pid, NULL, NULL);
                free_parse(input_parse);
                return 0;
            }
            else if (!strcmp(input_parse[0], "kill"))
            {
                ptrace(PTRACE_KILL, pid, NULL, NULL);
            }
            else if (!strcmp(input_parse[0], "register"))
            {
                print_register(regs);
            }
            else if (!strcmp(input_parse[0], "x") && nbArg == 3)
            {
                int countMem = atoi(input_parse[1]);
                void *ptrMem = (void *)strtol(input_parse[2], NULL, 16);
                print_memdump(1, countMem, ptrMem, pid);
            }
            else
            {
                printf("Bad argument !\n");
            }
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
    free_parse(input_parse);
    return 0;
}

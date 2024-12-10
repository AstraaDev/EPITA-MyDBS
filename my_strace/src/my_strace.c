#include "my_strace.h"

#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

char *get_syscall_name(long syscall_number)
{
    int syscall_count = sizeof(syscall_names) / sizeof(char *);
    return (syscall_number < 0 || syscall_number >= syscall_count)
        ? "Unknown syscall"
        : syscall_names[syscall_number];
}

SysCall *getArgSyscall(char *name)
{
    for (int i = 0; i < 80; i++)
    {
        if (!strcmp(sysArgList[i].name, name))
        {
            return &sysArgList[i];
        }
    }
    return NULL;
}

void printSyscallArgs(SysCall *sysArg, struct user_regs_struct regs,
                      int syscall_number, int pid)
{
    if (sysArg)
    {
        fprintf(stderr, "%s(", sysArg->name);
        for (int i = 0; i < sysArg->nbArg; i++)
        {
            long long int argVal;
            switch (i)
            {
            case 0:
                argVal = regs.rdi;
                break;
            case 1:
                argVal = regs.rsi;
                break;
            case 2:
                argVal = regs.rdx;
                break;
            case 3:
                argVal = regs.rcx;
                break;
            case 4:
                argVal = regs.r8;
                break;
            case 5:
                argVal = regs.r9;
                break;
            default:
                argVal = 0;
                break;
            }
            info data = { argVal, pid };
            if (i)
            {
                fprintf(stderr, ", %s = ", sysArg->args[i].name);
                sysArg->args[i].print_fonction(data);
            }
            else
            {
                fprintf(stderr, "%s = ", sysArg->args[i].name);
                sysArg->args[i].print_fonction(data);
            }
        }
        fprintf(stderr, ") = %lld\n", regs.rax);
    }
    else
    {
        fprintf(stderr, "%s() = %lld\n", get_syscall_name(syscall_number),
                regs.rax);
    }
    return;
}

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 2)
    {
        fprintf(stderr, "my_strace: Usage: ./my_strace <file_path>\n");
        return 1;
    }

    int pid = fork();
    switch (pid)
    {
    case -1:
        fprintf(stderr, "my_strace: Error: Fork failed");
        return 2;
    case 0:
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execve(argv[1], argv + 1, envp);
    default:
        break;
    }

    int status = 0;
    struct user_regs_struct regs = { 0 };
    int syscall_in_progress = 0;

    waitpid(pid, &status, 0);

    while (1)
    {
        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
            break;

        ptrace(PTRACE_GETREGS, pid, NULL, &regs);

        long syscall_number = regs.orig_rax;

        if (syscall_in_progress)
        {
            printSyscallArgs(getArgSyscall(get_syscall_name(syscall_number)),
                             regs, syscall_number, pid);
            syscall_in_progress = 0;
        }
        else
            syscall_in_progress = 1;
    }

    fprintf(stderr, "program exited with code %d\n", WEXITSTATUS(status));
    return 0;
}

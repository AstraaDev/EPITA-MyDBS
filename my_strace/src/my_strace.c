#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include "my_strace.h"

char *get_syscall_name(long syscall_number)
{
    int syscall_count = sizeof(syscall_names) / sizeof(syscall_names[0]);
    return (syscall_number < 0 || syscall_number >= syscall_count) ? "Unknown syscall" : syscall_names[syscall_number];
}

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 2)
    {
	fprintf(stderr, "my_strace: Usage: ./my_strace <file_path>\n");
        return 1;
    }

    int status = 0;
    long pt = 0;
    struct user_regs_struct regs = { 0 };
    int pid = fork();

    switch (pid)
    {
        case -1:
            return 2;
        case 0:
            ptrace(PTRACE_TRACEME, 0, NULL, NULL);
            execve(argv[1], argv + 1, envp);
        default:
            break;
    }
    
    while (1)
    {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            break;

        pt = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);
        ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	printf("%s() = %lld\n", get_syscall_name(pt), regs.rax);

        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    }
    
    printf("program exited with code %d\n", 0);
    return 0;
}

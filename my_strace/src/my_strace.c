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
            fprintf(stderr, "%s() = %lld\n", get_syscall_name(syscall_number), regs.rax);
    	    syscall_in_progress = 0;
	}
	else
	    syscall_in_progress = 1;
    }
    
    fprintf(stderr, "program exited with code %d\n", WEXITSTATUS(status));
    return 0;
}

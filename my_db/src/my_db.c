#include "my_db.h"

#include "utils.h"

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
            fgets(user_input, 4096, stdin);
            if (input_parse)
            {
                free_parse(input_parse);
                nbArg = 0;
            }

            input_parse = parser(user_input, &nbArg);

            if (input_parse[0] == NULL)
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
            else if (!strcmp(input_parse[0], "d") && nbArg == 3)
            {
                int countMem = atoi(input_parse[1]);
                void *ptrMem = (void *)strtol(input_parse[2], NULL, 16);
                print_memdump(2, countMem, ptrMem, pid);
            }
            else if (!strcmp(input_parse[0], "u") && nbArg == 3)
            {
                int countMem = atoi(input_parse[1]);
                void *ptrMem = (void *)strtol(input_parse[2], NULL, 16);
                print_memdump(3, countMem, ptrMem, pid);
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

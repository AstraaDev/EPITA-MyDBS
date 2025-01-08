#include "my_db.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>

#include "commands.h"
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

    struct brk_fifo *brkfifo = fifo_init();

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
        else if (!strcmp(input_parse[0], "continue") && nbArg == 1)
        {
            if (prog_continue(pid, &regs, brkfifo->head))
                break;
        }
        else if (!strcmp(input_parse[0], "quit") && nbArg == 1)
        {
            ptrace(PTRACE_KILL, pid, NULL, NULL);
            free_parse(input_parse);
            fifo_destroy(brkfifo);
            return 0;
        }
        else if (!strcmp(input_parse[0], "kill") && nbArg == 1)
        {
            ptrace(PTRACE_KILL, pid, NULL, NULL);
        }
        else if (!strcmp(input_parse[0], "register") && nbArg == 1)
        {
            prog_register(pid, &regs);
        }
        else if ((!strcmp(input_parse[0], "x") || !strcmp(input_parse[0], "d")
                  || !strcmp(input_parse[0], "u"))
                 && nbArg == 3)
        {
            prog_memdump(input_parse, pid);
        }
        else if ((!strcmp(input_parse[0], "next"))
                 && (nbArg == 1 || nbArg == 2))
        {
            if (prog_next(input_parse, nbArg, pid, &syscall_number, &regs,
                          brkfifo->head))
                break;
        }
        else if ((!strcmp(input_parse[0], "break")
                  || !strcmp(input_parse[0], "br")
                  || !strcmp(input_parse[0], "b"))
                 && nbArg == 2)
        {
            prog_break(input_parse, pid, brkfifo);
        }
        else if (!strcmp(input_parse[0], "blist") && nbArg == 1)
        {
            prog_blist(brkfifo);
        }
        else if (!strcmp(input_parse[0], "bdel") && nbArg == 2)
        {
            prog_bdel(input_parse, pid, brkfifo, &regs);
        }
        else if (!strcmp(input_parse[0], "help"))
        {
            prog_help();
        }
        else if (!strcmp(input_parse[0], "bt") && nbArg == 1)
        {
            prog_backtrace(pid, &regs);
        }
        else
        {
            printf("Bad argument !\n");
        }
    }

    fprintf(stderr, "program exited with code %d\n", WEXITSTATUS(status));
    free_parse(input_parse);
    fifo_destroy(brkfifo);
    return 0;
}

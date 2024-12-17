#include "my_db.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>

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
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);

            struct brk_struct *blist = brkfifo->head;
            while (blist != NULL)
            {
                if ((unsigned long)regs.rip - 1 == (unsigned long)blist->addr)
                {
                    ptrace(PTRACE_POKETEXT, pid, blist->addr, blist->oldVal);

                    regs.rip = (long long unsigned int)blist->addr;

                    ptrace(PTRACE_SETREGS, pid, NULL, &regs);

                    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);

                    waitpid(pid, &status, 0);

                    unsigned long br_value = (blist->oldVal & ~0xFF) | 0xCC;

                    ptrace(PTRACE_POKETEXT, pid, blist->addr, br_value);

                    break;
                }
                blist = blist->next;
            }

            ptrace(PTRACE_CONT, pid, NULL, NULL);
            waitpid(pid, &status, 0);

            if (WIFEXITED(status))
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
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
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
        else if ((!strcmp(input_parse[0], "next"))
                 && (nbArg == 1 || nbArg == 2))
        {
            int countStep;

            switch (nbArg)
            {
            case 1:
                countStep = 1;
                break;
            case 2:
                countStep = atoi(input_parse[1]);
                break;
            }

            int currentStep = 0;

            while (1)
            {
                if (currentStep == countStep)
                    break;

                ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
                waitpid(pid, &status, 0);

                syscall_number = regs.orig_rax;

                if (WIFEXITED(status))
                    break;

                if (syscall_number == -1)
                    break;

                currentStep++;
            }
            if (WIFEXITED(status))
                break;
        }

        else if ((!strcmp(input_parse[0], "break")
                  || !strcmp(input_parse[0], "b"))
                 && nbArg == 2)
        {
            char *brksymbol = malloc(sizeof(char) * strlen(input_parse[1]) + 1);
            strcpy(brksymbol, input_parse[1]);

            void *ptr = get_ptr_func(input_parse[1], pid);

            if (!ptr)
            {
                free(brksymbol);
                printf("Bad argument !\n");
                continue;
            }

            unsigned long aligned_address =
                0x555555554000 + (unsigned long)ptr + 8;

            errno = 0;
            long data =
                ptrace(PTRACE_PEEKTEXT, pid, (void *)aligned_address, NULL);

            unsigned long br_value = (data & ~0xFF) | 0xCC;

            ptrace(PTRACE_POKETEXT, pid, aligned_address, br_value);

            fifo_push(brkfifo, (void *)aligned_address, brksymbol, data);
        }
        else if (!strcmp(input_parse[0], "blist") && nbArg == 1)
        {
            int i = 0;
            struct brk_struct *blist = brkfifo->head;
            while (blist != NULL)
            {
                printf("%d %p %s\n", i + 1, blist->addr, blist->symbol);
                blist = blist->next;
                i++;
            }
        }
        else if (!strcmp(input_parse[0], "bdel") && nbArg == 2)
        {
            size_t index = atoi(input_parse[1]);
            if (index <= 0 || index > brkfifo->size)
                printf("Bad argument !\n");
            else
            {
                struct brk_struct *brk = fifo_get(brkfifo, index);

                ptrace(PTRACE_GETREGS, pid, NULL, &regs);

                if ((unsigned long)regs.rip - 1 == (unsigned long)brk->addr)
                {
                    regs.rip = (long long unsigned int)brk->addr;

                    ptrace(PTRACE_SETREGS, pid, NULL, &regs);
                }

                ptrace(PTRACE_POKETEXT, pid, brk->addr, brk->oldVal);

                fifo_pop(brkfifo, index);
            }
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

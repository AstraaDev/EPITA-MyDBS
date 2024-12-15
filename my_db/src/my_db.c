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

    int stepFlag = 0;
    int countStep = 0;
    int currentStep = 0;

    struct brk_fifo *brkfifo = fifo_init();

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
            else if (!strcmp(input_parse[0], "continue") && nbArg == 1)
            {
                break;
            }
            else if (!strcmp(input_parse[0], "quit") && nbArg == 1)
            {
                ptrace(PTRACE_KILL, pid, NULL, NULL);
                free_parse(input_parse);
                return 0;
            }
            else if (!strcmp(input_parse[0], "kill") && nbArg == 1)
            {
                ptrace(PTRACE_KILL, pid, NULL, NULL);
            }
            else if (!strcmp(input_parse[0], "register") && nbArg == 1)
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
            else if ((!strcmp(input_parse[0], "next"))
                     && (nbArg == 1 || nbArg == 2))
            {
                switch (nbArg)
                {
                case 1:
                    stepFlag = 1;
                    countStep = 1;
                    break;
                case 2:
                    stepFlag = 1;
                    countStep = atoi(input_parse[1]);
                    break;
                }
                break;
            }
	    else if ((!strcmp(input_parse[0], "break") || !strcmp(input_parse[0], "b")) && nbArg == 2)
	    {
		char *brksymbol = malloc(sizeof(char) * strlen(input_parse[1]) + 1);
		strcpy(brksymbol, input_parse[1]);
		fifo_push(brkfifo, 0, brksymbol);
	    }
	    else if (!strcmp(input_parse[0], "blist") && nbArg == 1)
	    {
		int i = 0;
		struct brk_struct *blist = brkfifo->head;
		while (blist != NULL)
		{
		    printf("%d 0x%lx %s\n", i+1, blist->addr, blist->symbol);
		    blist = blist->next;
		    i++;
		}
	    }
            else
            {
                printf("Bad argument !\n");
            }
        }

        currentStep = 0;

        while (1)
        {
            if (stepFlag)
                if (currentStep == countStep)
                    break;

            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
            waitpid(pid, &status, 0);

            syscall_number = regs.orig_rax;

            if (WIFEXITED(status))
                break;

            if (syscall_number == -1)
                break;

            ptrace(PTRACE_GETREGS, pid, NULL, &regs);

            currentStep++;
        }
        stepFlag = 0;
        countStep = 0;
    }

    fprintf(stderr, "program exited with code %d\n", WEXITSTATUS(status));
    free_parse(input_parse);
    return 0;
}

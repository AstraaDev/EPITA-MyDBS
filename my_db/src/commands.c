#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>

int prog_next(char **input_parse, int nbArg, int pid, struct brk_struct *blist,
              int *status)
{
    struct user_regs_struct regs = { 0 };
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
        if (currentStep >= countStep)
            break;

        ptrace(PTRACE_GETREGS, pid, NULL, &regs);

        int syscall_number = regs.rax;

        if (syscall_number == -1)
        {
            while (blist != NULL)
            {
                if ((unsigned long)regs.rip - 1 == (unsigned long)blist->addr)
                {
                    ptrace(PTRACE_POKETEXT, pid, blist->addr, blist->oldVal);

                    regs.rip = (long long unsigned int)blist->addr;

                    ptrace(PTRACE_SETREGS, pid, NULL, &regs);

                    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);

                    waitpid(pid, status, 0);

                    unsigned long br_value = (blist->oldVal & ~0xFF) | 0xCC;

                    ptrace(PTRACE_POKETEXT, pid, blist->addr, br_value);

                    break;
                }
                blist = blist->next;
            }
        }

        ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
        waitpid(pid, status, 0);

        if (WIFEXITED(*status))
            break;

        // prog_backtrace(pid, regs);

        currentStep++;
    }
    if (WIFEXITED(*status))
        return 1;
    return 0;
}

int prog_continue(int pid, struct brk_struct *blist, int *status)
{
    struct user_regs_struct regs = { 0 };
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);

    while (blist != NULL)
    {
        if ((unsigned long)regs.rip - 1 == (unsigned long)blist->addr)
        {
            ptrace(PTRACE_POKETEXT, pid, blist->addr, blist->oldVal);

            regs.rip = (long long unsigned int)blist->addr;

            ptrace(PTRACE_SETREGS, pid, NULL, &regs);

            ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);

            waitpid(pid, status, 0);

            unsigned long br_value = (blist->oldVal & ~0xFF) | 0xCC;

            ptrace(PTRACE_POKETEXT, pid, blist->addr, br_value);

            break;
        }
        blist = blist->next;
    }

    ptrace(PTRACE_CONT, pid, NULL, NULL);
    waitpid(pid, status, 0);

    if (WIFEXITED(*status))
        return 1;
    return 0;
}

void prog_register(int pid)
{
    struct user_regs_struct regs = { 0 };
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    print_register(regs);
}

void prog_memdump(char **input_parse, int pid)
{
    int countMem = atoi(input_parse[1]);

    void *ptrMem;

    if (input_parse[2][0] == '$')
    {
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, NULL, &regs);

        if (!strcmp(input_parse[2], "$rax"))
        {
            ptrMem = (void *)regs.rax;
        }
        else if (!strcmp(input_parse[2], "$rbx"))
        {
            ptrMem = (void *)regs.rbx;
        }
        else if (!strcmp(input_parse[2], "$rcx"))
        {
            ptrMem = (void *)regs.rcx;
        }
        else if (!strcmp(input_parse[2], "$rdx"))
        {
            ptrMem = (void *)regs.rdx;
        }
        else if (!strcmp(input_parse[2], "$rsi"))
        {
            ptrMem = (void *)regs.rsi;
        }
        else if (!strcmp(input_parse[2], "$rdi"))
        {
            ptrMem = (void *)regs.rdi;
        }
        else if (!strcmp(input_parse[2], "$rbp"))
        {
            ptrMem = (void *)regs.rbp;
        }
        else if (!strcmp(input_parse[2], "$rsp"))
        {
            ptrMem = (void *)regs.rsp;
        }
        else if (!strcmp(input_parse[2], "$r8"))
        {
            ptrMem = (void *)regs.r8;
        }
        else if (!strcmp(input_parse[2], "$r9"))
        {
            ptrMem = (void *)regs.r9;
        }
        else if (!strcmp(input_parse[2], "$r10"))
        {
            ptrMem = (void *)regs.r10;
        }
        else if (!strcmp(input_parse[2], "$r11"))
        {
            ptrMem = (void *)regs.r11;
        }
        else if (!strcmp(input_parse[2], "$r12"))
        {
            ptrMem = (void *)regs.r12;
        }
        else if (!strcmp(input_parse[2], "$r13"))
        {
            ptrMem = (void *)regs.r13;
        }
        else if (!strcmp(input_parse[2], "$r14"))
        {
            ptrMem = (void *)regs.r14;
        }
        else if (!strcmp(input_parse[2], "$r15"))
        {
            ptrMem = (void *)regs.r15;
        }
        else if (!strcmp(input_parse[2], "$rip"))
        {
            ptrMem = (void *)regs.rip;
        }
        else
        {
            printf("invalid register !!!\n");
            return;
        }
    }
    else if (input_parse[2][0] == '0'
             && (input_parse[2][1] == 'x' || input_parse[2][1] == 'X'))
    {
        ptrMem = (void *)strtol(input_parse[2], NULL, 16);
    }
    else
    {
        ptrMem = (void *)atol(input_parse[2]);
    }

    switch (input_parse[0][0])
    {
    case 'x':
        print_memdump(1, countMem, ptrMem, pid);
        break;
    case 'd':
        print_memdump(2, countMem, ptrMem, pid);
        break;
    case 'u':
        print_memdump(3, countMem, ptrMem, pid);
        break;
    }
}

void prog_break(char **input_parse, int pid, struct brk_fifo *brkfifo)
{
    char *brksymbol = malloc(sizeof(char) * strlen(input_parse[1]) + 1);
    strcpy(brksymbol, input_parse[1]);

    void *ptr = get_ptr_func(input_parse[1], pid);

    if (!ptr)
    {
        ptr = (void *)strtol(input_parse[1], NULL, 16);
    }

    unsigned long aligned_address = 0x555555554000 + (unsigned long)ptr + 8;

    if (aligned_address == 0x555555554008)
    {
        printf("Name or pointeur invalide.\n");
        free(brksymbol);
        return;
    }

    errno = 0;
    long data = ptrace(PTRACE_PEEKTEXT, pid, (void *)aligned_address, NULL);

    unsigned long br_value = (data & ~0xFF) | 0xCC;

    ptrace(PTRACE_POKETEXT, pid, aligned_address, br_value);

    fifo_push(brkfifo, (void *)aligned_address, brksymbol, data);
}

void prog_blist(struct brk_fifo *brkfifo)
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
void prog_bdel(char **input_parse, int pid, struct brk_fifo *brkfifo)
{
    struct user_regs_struct regs = { 0 };
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

void prog_help()
{
    printf("+---------------------------------------+\n");
    printf("|             COMMANDS LIST             |\n");
    printf("+---------------------------------------+\n");
    printf("| next     -> Passer à l'étape suivante |\n");
    printf("| continue -> Continuer le processus    |\n");
    printf("| register -> Dump les registres        |\n");
    printf("| break    -> Interrompre le processus  |\n");
    printf("| blist    -> Liste des breakpoints     |\n");
    printf("| bdel     -> Supprimer un breakpoint   |\n");
    printf("| x        -> Dump en hexa une valeur   |\n");
    printf("| u        -> Dump en signed int        |\n");
    printf("| d        -> Dump en int               |\n");
    printf("| kill     -> Kill le programme fils    |\n");
    printf("| quit     -> Quitte le programme       |\n");
    printf("+---------------------------------------+\n");
}

void prog_backtrace(int pid)
{
    struct user_regs_struct regs = { 0 };
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    uintptr_t rbp = regs.rbp;

    uintptr_t return_address;

    while (return_address != 0)
    {
        return_address = ptrace(PTRACE_PEEKDATA, pid, rbp, NULL);
        printf("Return address: %lx\n", return_address);

        rbp = rbp - sizeof(uintptr_t);
    }
}

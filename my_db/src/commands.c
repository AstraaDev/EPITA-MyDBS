#include "commands.h"

int prog_next(char **input_parse, int nbArg, int pid, long *syscall_number,
              struct user_regs_struct *regs)
{
    int status = 0;
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

        *syscall_number = regs->orig_rax;

        if (WIFEXITED(status))
            break;

        if (*syscall_number == -1)
            break;

        currentStep++;
    }
    if (WIFEXITED(status))
        return 1;
    return 0;
}

int prog_continue(int pid, struct user_regs_struct *regs,
                  struct brk_struct *blist)
{
    int status = 0;
    ptrace(PTRACE_GETREGS, pid, NULL, regs);

    while (blist != NULL)
    {
        if ((unsigned long)regs->rip - 1 == (unsigned long)blist->addr)
        {
            ptrace(PTRACE_POKETEXT, pid, blist->addr, blist->oldVal);

            regs->rip = (long long unsigned int)blist->addr;

            ptrace(PTRACE_SETREGS, pid, NULL, regs);

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
        return 1;
    return 0;
}

void prog_register(int pid, struct user_regs_struct *regs)
{
    ptrace(PTRACE_GETREGS, pid, NULL, regs);
    print_register(*regs);
}

void prog_memdump(char **input_parse, int pid)
{
    int countMem = atoi(input_parse[1]);
    void *ptrMem = (void *)strtol(input_parse[2], NULL, 16);

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
void prog_bdel(char **input_parse, int pid, struct brk_fifo *brkfifo,
               struct user_regs_struct *regs)
{
    size_t index = atoi(input_parse[1]);
    if (index <= 0 || index > brkfifo->size)
        printf("Bad argument !\n");
    else
    {
        struct brk_struct *brk = fifo_get(brkfifo, index);

        ptrace(PTRACE_GETREGS, pid, NULL, regs);

        if ((unsigned long)regs->rip - 1 == (unsigned long)brk->addr)
        {
            regs->rip = (long long unsigned int)brk->addr;

            ptrace(PTRACE_SETREGS, pid, NULL, regs);
        }

        ptrace(PTRACE_POKETEXT, pid, brk->addr, brk->oldVal);

        fifo_pop(brkfifo, index);
    }
}

void prog_help();

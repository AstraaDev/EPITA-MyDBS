#include "utils.h"

void free_parse(char **parse)
{
    int i = 0;
    while (parse[i] != NULL)
    {
        free(parse[i++]);
    }
    free(parse);
}

char **parser(char *input, int *nbArg)
{
    int size = 5;
    char **parse_input = calloc(size, sizeof(char *));

    if (input[0] == '\n')
        return parse_input;

    int i = 1;
    int count = 1;
    while (input[i - 1] != '\n')
    {
        if (input[i] == ' ' || input[i] == '\n')
        {
            char *new = calloc(count + 1, sizeof(char));
            memcpy(new, &input[i - count], count);
            parse_input[(*nbArg)++] = new;
            if (size <= (*nbArg))
            {
                size *= 2;
                parse_input = realloc(parse_input, size * (sizeof(char *)));
            }
            count = 0;
        }
        else
        {
            count++;
        }
        i++;
    }
    return parse_input;
}

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

        switch (flag)
        {
        case 1:
            printf("%p 0x%llx\n", ptrTmp++, val);
            break;
        case 2:
            printf("%p %lld\n", ptrTmp++, (long long)val);
            break;
        case 3:
            printf("%p %llu\n", ptrTmp++, (unsigned long long)val);
            break;
        default:
            printf("%p 0x%llx\n", ptrTmp++, val);
            break;
        }
    }
}

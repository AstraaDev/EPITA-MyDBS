#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct brk_fifo *fifo_init(void)
{
    struct brk_fifo *new_queue = malloc(sizeof(struct brk_fifo));

    if (!new_queue)
        return NULL;

    new_queue->head = NULL;
    new_queue->tail = NULL;
    new_queue->size = 0;

    return new_queue;
}

void fifo_push(struct brk_fifo *fifo, void *addr, char *symbol,
               unsigned long oldVal)
{
    struct brk_struct *to_add = malloc(sizeof(struct brk_struct));

    to_add->addr = addr;
    to_add->symbol = symbol;
    to_add->oldVal = oldVal;
    to_add->next = NULL;

    if (fifo->tail == NULL)
    {
        fifo->head = to_add;
        fifo->tail = to_add;
    }
    else
    {
        fifo->tail->next = to_add;
        fifo->tail = to_add;
    }

    fifo->size++;
}

void fifo_pop(struct brk_fifo *fifo, size_t index)
{
    if (fifo == NULL || fifo->head == NULL)
        return;

    struct brk_struct *current = fifo->head;
    struct brk_struct *previous = NULL;

    for (size_t i = 0; i < index - 1; i++)
    {
        previous = current;
        current = current->next;
    }

    if (previous == NULL)
    {
        fifo->head = current->next;
        if (fifo->head == NULL)
            fifo->tail = NULL;
    }
    else
    {
        previous->next = current->next;
        if (current->next == NULL)
            fifo->tail = previous;
    }

    free(current->symbol);
    free(current);
    fifo->size--;
}

struct brk_struct *fifo_get(struct brk_fifo *fifo, size_t index)
{
    if (fifo == NULL || fifo->head == NULL)
        return NULL;

    struct brk_struct *current = fifo->head;

    for (size_t i = 0; i < index - 1; i++)
    {
        current = current->next;
    }

    return current;
}

void fifo_destroy(struct brk_fifo *fifo)
{
    struct brk_struct *current = fifo->head;

    while (current)
    {
        struct brk_struct *to_free = current;
        current = current->next;
        free(to_free->symbol);
        free(to_free);
    }

    free(fifo);
}

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
            printf("%p 0x%llx\n", ptrTmp, val);
            break;
        case 2:
            printf("%p %lld\n", ptrTmp, (long long)val);
            break;
        case 3:
            printf("%p %llu\n", ptrTmp, (unsigned long long)val);
            break;
        default:
            printf("%p 0x%llx\n", ptrTmp, val);
            break;
        }
        ptrTmp += sizeof(uintptr_t);
    }
}

void *get_ptr_func(char *name, int pid)
{
    char exe_path[256];
    snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);

    int fd = open(exe_path, O_RDONLY);

    struct stat s = { 0 };
    if ((stat(exe_path, &s)) == -1)
    {
        close(fd);
    }

    void *ptr = NULL;

    Elf64_Ehdr *elf_header =
        mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    Elf64_Shdr *section_headers =
        (Elf64_Shdr *)((char *)elf_header + elf_header->e_shoff);

    for (int i = 0; i < elf_header->e_shnum; i++)
    {
        if (section_headers[i].sh_type == SHT_SYMTAB)
        {
            Elf64_Shdr strtab_header =
                section_headers[section_headers[i].sh_link];
            char *strtab = (char *)elf_header + strtab_header.sh_offset;

            Elf64_Sym *symbols = (Elf64_Sym *)((char *)elf_header
                                               + section_headers[i].sh_offset);
            int count = section_headers[i].sh_size / sizeof(Elf64_Sym);

            for (int j = 0; j < count; j++)
            {
                char *symbol_name = &strtab[symbols[j].st_name];

                if (symbols[j].st_shndx > elf_header->e_ehsize)
                    continue;

                if (!strcmp(symbol_name, name))
                    ptr = (void *)symbols[j].st_value;
            }
        }
    }

    close(fd);
    return ptr;
}

void over_br(int pid, struct brk_struct *blist, int *status)
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
}

int br_check(struct brk_struct *blist, struct user_regs_struct regs)
{
    while (blist != NULL)
    {
        if ((unsigned long)regs.rip - 1 == (unsigned long)blist->addr)
        {
            return 1;
        }
        blist = blist->next;
    }
    return 0;
}

#ifndef UTILS_H
#define UTILS_H

#include <elf.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

struct brk_struct
{
    void *addr;
    char *symbol;
    unsigned long oldVal;
    struct brk_struct *next;
};

struct brk_fifo
{
    struct brk_struct *head;
    struct brk_struct *tail;
    size_t size;
};

struct brk_fifo *fifo_init(void);
void fifo_push(struct brk_fifo *fifo, void *addr, char *symbol,
               unsigned long oldVal);
void fifo_pop(struct brk_fifo *fifo, size_t index);
struct brk_struct *fifo_get(struct brk_fifo *fifo, size_t index);
void fifo_destroy(struct brk_fifo *fifo);
char **parser(char *input, int *nbArg);
void free_parse(char **parse);
void print_memdump(int flag, int count, void *ptr, int pid);
void print_register(struct user_regs_struct regs);
void *get_ptr_func(char *name, int pid);
void over_br(int pid, struct brk_struct *blist, int *status);
int br_check(struct brk_struct *blist, struct user_regs_struct regs);
long long unsigned call_check(struct user_regs_struct *regs, int pid);

#endif /* !UTILS_H */

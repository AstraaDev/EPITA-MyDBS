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

char **parser(char *input, int *nbArg);
void free_parse(char **parse);
void print_memdump(int flag, int count, void *ptr, int pid);
void print_register(struct user_regs_struct regs);
void get_ptr_func(char *name, int pid);

#endif /* !UTILS_H */

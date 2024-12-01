#ifndef MY_NM_H
#define MY_NM_H

#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int check_elf(const char *filename);
void print_symbol(Elf64_Sym *symbol, char *name_str);

#endif /* !MY_NM_H */

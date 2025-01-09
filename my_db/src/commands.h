#ifndef COMMANDS_H
#define COMMANDS_H

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

int prog_next(char **input_parse, int nbArg, int pid, struct brk_struct *blist,
              int *status);
int prog_continue(int pid, struct brk_struct *blist, int *status);
void prog_register(int pid);
void prog_memdump(char **input_parse, int pid);
void prog_break(char **input_parse, int pid, struct brk_fifo *brkfifo);
void prog_blist(struct brk_fifo *brkfifo);
void prog_bdel(char **input_parse, int pid, struct brk_fifo *brkfifo);
void prog_help();
void prog_backtrace(int pid);

#endif /* !COMMANDS_H */

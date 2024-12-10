#include "utils.h"

#include <stdio.h>
#include <sys/ptrace.h>

void print_int(info data)
{
    fprintf(stderr, "%d", (int)data.ptr);
}

void print_long(info data)
{
    fprintf(stderr, "%ld", (long)data.ptr);
}

void print_ptr(info data)
{
    if ((void *)data.ptr != NULL)
    {
        fprintf(stderr, "%p", (void *)data.ptr);
    }
    else
    {
        fprintf(stderr, "0x0");
    }
}

void print_char(info data)
{
    fprintf(stderr, "%c", (char)data.ptr);
}

void print_str(info data)
{
    fprintf(stderr, "\"");

    char s = 1;
    char *ptr = (void *)data.ptr;
    while (s != '\0')
    {
        s = (char)ptrace(PTRACE_PEEKDATA, data.pid, ptr++, NULL);
        fprintf(stderr, "%c", s);
    }
    fprintf(stderr, "\"");
}

void print_hexa(info data)
{
    fprintf(stderr, "%x", (int)data.ptr);
}

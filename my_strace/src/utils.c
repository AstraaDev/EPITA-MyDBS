#include "utils.h"

#include <stdio.h>
#include <sys/ptrace.h>
#include <fcntl.h>
#include <string.h>

#ifndef O_CLOEXEC
#define O_CLOEXEC 02000000
#endif

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

void print_open_flags(info data)
{
    long long int flags = data.ptr;

    char buffer[128] = {0};

    if ((flags & O_ACCMODE) == O_RDONLY)
	strcat(buffer, "O_RDONLY|");
    if ((flags &  O_ACCMODE) == O_WRONLY)
	strcat(buffer, "O_WRONLY|");
    if ((flags &  O_ACCMODE) == O_RDWR)
	strcat(buffer, "O_RDWR|");
    if (flags & O_CREAT)
	strcat(buffer, "O_CREAT|");
    if (flags & O_EXCL)
	strcat(buffer, "O_EXCL|");
    if (flags & O_NOCTTY)
	strcat(buffer, "O_NOCTTY|");
    if (flags & O_TRUNC)
	strcat(buffer, "O_TRUNC|");
    if (flags & O_APPEND)
	strcat(buffer, "O_APPEND|");
    if (flags & O_NONBLOCK)
	strcat(buffer, "O_NONBLOCK|");
    if (flags & O_CLOEXEC)
	strcat(buffer, "O_CLOEXEC|");

    if (strlen(buffer) > 0)
        buffer[strlen(buffer) - 1] = '\0';
    else
	strcat(buffer, "UNDEFINED");

    fprintf(stderr, "%s", buffer);
}

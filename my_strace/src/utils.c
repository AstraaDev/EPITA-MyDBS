#include "utils.h"

#include <stdio.h>

void print_int(long long int ptr)
{
    fprintf(stderr, "%d", (int)ptr);
}

void print_long(long long int ptr)
{
    fprintf(stderr, "%ld", (long)ptr);
}

void print_ptr(long long int ptr)
{
    fprintf(stderr, "%p", (void *)ptr);
}

void print_char(long long int ptr)
{
    fprintf(stderr, "%c", (char)ptr);
}

void print_str(long long int ptr)
{
    (void)ptr;
    fprintf(stderr, "string");
}

void print_hexa(long long int ptr)
{
    fprintf(stderr, "%x", (int)ptr);
}

#ifndef UTILS_H
#define UTILS_H

typedef struct
{
    long long int ptr;
    int pid;
} info;

void print_int(info data);
void print_long(info data);
void print_ptr(info data);
void print_char(info data);
void print_str(info data);
void print_hexa(info data);
void print_open_flags(info data);

#endif /* !UTILS_H */

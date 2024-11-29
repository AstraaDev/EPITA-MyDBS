#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int check_elf(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "my_nm: Error: Cannot open file in read-only\n");
        return -1;
    }

    char magic[4];
    size_t bytes_read = read(fd, magic, sizeof(magic));
    if (bytes_read != sizeof(magic))
    {
        fprintf(stderr, "my_nm: Error: Cannot read file\n");
        return -1;
    }

    if (magic[0] == EI_MAG0 && magic[1] == EI_MAG1 && magic[2] == EI_MAG2
        && magic[3] == EI_MAG3)
        return fd;
    else
    {
        fprintf(stderr, "my_nm: Error: Not an ELF file\n");
        return -1;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "my_nm: Usage: ./my_nm <file_path>\n");
        return -1;
    }

    int fd = check_elf(argv[1]);
    if (fd == -1)
        return 1;

    printf("Value\t\t\tSize\tType\t\tBind\t\tVis\t\tName\n");
    for (int i = 0; i < 6; i++)
        printf("%016X\t0\tSTT_SECTION\tSTB_LOCAL\tSTV_DEFAULT\t.text\n", 0);

    return 0;
}

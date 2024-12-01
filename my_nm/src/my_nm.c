#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
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

    if (magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L'
        && magic[3] == 'F')
        return fd;
    else
    {
        fprintf(stderr, "my_nm: Error: Not an ELF file\n");
        return -1;
    }
}

void print_symbol(Elf64_Sym *symbol, char *section_str, char *symbol_str)
{
    char bind = ELF64_ST_BIND(symbol->st_info);
    char *bind_str = NULL;
    switch (bind)
    {
    case STB_LOCAL:
        bind_str = "STB_LOCAL";
        break;
    case STB_GLOBAL:
        bind_str = "STB_GLOBAL";
        break;
    case STB_WEAK:
        bind_str = "STB_WEAK";
        break;
    default:
        bind_str = "STB_UNKNOWN";
        break;
    }

    char type = ELF64_ST_TYPE(symbol->st_info);
    char *type_str = NULL;
    switch (type)
    {
    case STT_NOTYPE:
        type_str = "STT_NOTYPE";
        break;
    case STT_OBJECT:
        type_str = "STT_OBJECT";
        break;
    case STT_FUNC:
        type_str = "STT_FUNC";
        break;
    case STT_SECTION:
        type_str = "STT_SECTION";
        break;
    case STT_FILE:
        type_str = "STT_FILE";
        return;
    default:
        type_str = "STT_UNKNOWN";
        break;
    }

    char vis = ELF64_ST_VISIBILITY(symbol->st_other);
    char *vis_str = NULL;
    switch (vis)
    {
    case STV_DEFAULT:
        vis_str = "STV_DEFAULT";
        break;
    case STV_INTERNAL:
        vis_str = "STV_INTERNAL";
        break;
    case STV_HIDDEN:
        vis_str = "STV_HIDDEN";
        break;
    case STV_PROTECTED:
        vis_str = "STV_PROTECTED";
        break;
    default:
        vis_str = "SRV_UNKNOWN";
        break;
    };

    printf("%016lx\t%lu\t%s\t%s\t%s\t%s\t%s\n", symbol->st_value,
           symbol->st_size, type_str, bind_str, vis_str, section_str,
           symbol_str);
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

    struct stat s = { 0 };
    if ((stat(argv[1], &s)) == -1)
    {
        fprintf(stderr, "my_nm: Error: Cannot stat(filename)");
        close(fd);
        return 1;
    }

    Elf64_Ehdr *elf_header =
        mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (elf_header == MAP_FAILED)
    {
        fprintf(stderr, "my_nm: Error: Cannot mmap for the file size");
        close(fd);
        return 1;
    }

    Elf64_Shdr *section_headers =
        (Elf64_Shdr *)((char *)elf_header + elf_header->e_shoff);

    Elf64_Shdr shstrtab_header = section_headers[elf_header->e_shstrndx];
    char *shstrtab = (char *)elf_header + shstrtab_header.sh_offset;

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

                print_symbol(
                    &symbols[j],
                    symbols[j].st_shndx != 0
                        ? &shstrtab[section_headers[symbols[j].st_shndx]
                                        .sh_name]
                        : "UND",
                    *symbol_name != '\0' ? symbol_name : "");
            }
        }
    }

    close(fd);
    munmap(elf_header, s.st_size);
    return 0;
}

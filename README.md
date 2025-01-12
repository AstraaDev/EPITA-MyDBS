# 🚀 My Debugging Suite - EPITA Project
```
                                 _nnnn_                      
                                dGGGGMMb     ,"""""""""""""""""""""""".
                               @p~qp~~qMb    |  La rage de vaincre !  |
                               M|@||@) M|   _;........................'
                               @,----.JM| -'
                              JS^\__/  qKL
                             dZP        qKRb
                            dZP          qKKb
                           fZP            SMMb
                           HZM            MMMM
                           FqM            MMMM
                         __| ".        |\dS"qML
                         |    `.       | `' \Zq
                        _)      \.___.,|     .'
                        \____   )MMMMMM|   .'
                             `-'       `--'
```

---

## 📖 Project Overview

**My Debugging Suite** is a project developed as part of the APP ING1 course at EPITA. The project introduces you to static and dynamic analysis of ELF binaries by implementing tools similar to `nm`, `strace`, and `gdb`.

The project consists of **three main tools** and one **bonus tool**:
1. **my_nm**: Static analysis of ELF symbols.
2. **my_strace**: Dynamic analysis of system calls.
3. **my_db**: A minimalist debugger.

---

## ⚠️  Note on Optional Exercises

As part of this project, our team has completed several optional exercises to enhance our learning experience. However, we would like to clarify that we do not wish for these exercises to be considered in our grading unless other teams have also completed them, thereby making these exercises mandatory for the entire class.

---

## 🛠️ Installation

### Prerequisites

- **System**: Linux (amd64)
- **Language**: C (C99 standard)
- **Compiler**: `cc`
- **Compilation Flags**:
    - `-std=c99 -pedantic -Wall -Wextra -Wvla -Werror`

### Clone the Repository

```bash
git clone <repository-url>
```

### Build the Project

```bash
make
```

---

## 🚀 Features

### 1. **my_nm** - Static ELF Symbol Analysis
**Usage**:
```bash
./my_nm /path/to/binary
```
**Description**:
- Displays the symbol table of an ELF file.

**Example**:
```bash
42sh$ ./my_nm example.o
0000000000000000	0	STT_NOTYPE	STB_LOCAL	STV_DEFAULT	UND
0000000000000000	0	STT_SECTION	STB_LOCAL	STV_DEFAULT	.text
0000000000000000	0	STT_SECTION	STB_LOCAL	STV_DEFAULT	.rodata
0000000000000000	24	STT_FUNC	STB_GLOBAL	STV_DEFAULT	.text	do_calc
0000000000000018	98	STT_FUNC	STB_GLOBAL	STV_DEFAULT	.text	main
0000000000000000	0	STT_NOTYPE	STB_GLOBAL	STV_DEFAULT	UND	printf
```

---

### 2. **my_strace** - System Call Analysis
**Usage**:
```bash
./my_strace /path/to/binary [forbidden_syscalls]
```
**Description**:
- Displays the list of system calls made by a program.
- Filtering of syscalls allowed (Thresholds 3 bonus)

**Example**:
```bash
42sh$ ./my_strace /bin/ls
brk(addr = 0x0) = 93824992378880
arch_prctl() = -22
mmap(addr = 0x0, length = 8192, prot = 3, flags = -134308681, fd = -1, offset = 0) = 140737353854976
access(pathname = "/etc/ld.so.preload", mode = 4) = -2
...
write(fd = 1, buf = 0x555555578500, count = 25) = 25
close(fd = 1) = 0
close(fd = 2) = 0
program exited with code 0
```
```bash
42sh$ ./my_strace /bin/ls read
brk(addr = 0x0) = 93824992378880
arch_prctl() = -22
mmap(addr = 0x0, length = 8192, prot = 3, flags = -134308681, fd = -1, offset = 0) = 140737353854976
access(pathname = "/etc/ld.so.preload", mode = 4) = -2
...
forbidden syscall blocked: read
close(fd = 3) = 0
/bin/ls: error while loading shared libraries: /lib/x86_64-linux-gnu/libselinux.so.1: cannot read file data: Operation not permitted
writev(fd = 2, iov = 0x7fffffffd040, iovcnt = 10) = 133
program exited with code 127
```

---

### 3. **my_db** - Minimalist Debugger
**Usage**:
```bash
./my_db /path/to/binary
```
**Description**:
- Provides basic commands to control a running program:
    - `help`: displays commands details.
    - `quit`: quit the debugger.
    - `kill`: kill the process being debugged.
    - `continue`: continue the process execution.
    - `registers`: display the registers list from the process being debugged.
    - `command count addr` : dump memory at a given address.
      - `x`, `d`, `u`: print, respectively, in hexadecimal, signed decimal or unsigned decimal.
      - `count`: is the number of 64 bits values you have to display.
      - `addr`: is the address (decimal or hexadecimal) from which you have to print. It can also be a symbol from the binary.
    - `break`: followed by an argument. This command puts a breakpoint to the given address. The argument must be an address (in hexadecimal or decimal) or a symbol from the binary (beware of the type, it should be a shame to put a breakpoint on a global variable).
    - `blist`: print the breakpoints list.
    - `bdel`: delete the breakpoint whose number is displayed in the list.
    - `bt`: displays Callstack but only the address, not the name of the function.

**Example**:
```bash
42sh$ ./my_db debugme
> continue
before breakpoint
> registers
rax: 0x14 rbx: 0x7fffffffde08
...
> quit
```

---

## 📂 Project Structure
<details>
<summary>42sh$ tree ./EPITA-MyDBS</summary>

```bash
mydbs/
├── my_db/
│   ├── Makefile
│   └── src/
│       ├── my_db.c
│       ├── my_db.h
│       ├── commands.c
│       ├── commands.h
│       ├── utils.c
│       └── utils.h
├── my_nm/
│   ├── Makefile
│   └── src/
│       ├── my_nm.c
│       └── my_nm.h
├── my_strace/
│   ├── Makefile
│   └── src/
│       ├── my_strace.c
│       ├── my_strace.h
│       ├── utils.c
│       └── utils.h
├── README.md
└── TODO.md
```
</details>

---

## 👨‍💻 Authors

- **Killian Heritier** - *Project Manager* - [killian.heritier@epita.fr](mailto:killian.heritier@epita.fr)
- **Paul Baudinot** - *Project Manager* - [paul.baudinot@epita.fr](mailto:paul.baudinot@epita.fr)

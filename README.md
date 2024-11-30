```
  ███▄ ▄███▓▓██   ██▓▓█████▄  ▄▄▄▄     ██████ 
▓██▒▀█▀ ██▒ ▒██  ██▒▒██▀ ██▌▓█████▄ ▒██    ▒ 
▓██    ▓██░  ▒██ ██░░██   █▌▒██▒ ▄██░ ▓██▄   
▒██    ▒██   ░ ▐██▓░░▓█▄   ▌▒██░█▀    ▒   ██▒
▒██▒   ░██▒  ░ ██▒▓░░▒████▓ ░▓█  ▀█▓▒██████▒▒
░ ▒░   ░  ░   ██▒▒▒  ▒▒▓  ▒ ░▒▓███▀▒▒ ▒▓▒ ▒ ░
░  ░      ░ ▓██ ░▒░  ░ ▒  ▒ ▒░▒   ░ ░ ░▒  ░ ░
░      ░    ▒ ▒ ░░   ░ ░  ░  ░    ░ ░  ░  ░  
       ░    ░ ░        ░     ░            ░  
            ░ ░      ░            ░          
```

# My Debugging Suite

## Description

**My Debugging Suite** is a project designed to analyze and manipulate ELF binaries and system-level structures in depth. This project is part of the APPING SYS1 coursework at EPITA and includes the following tools:

1. **my_nm**: Analyze symbols in an ELF file.
2. **my_strace**: Trace system calls made by a program.
3. **my_db**: A mini-debugger.
4. **my_prof** (Bonus): Profiling tool for functions in a binary.

---

## Features

### my_nm
- Display the symbol table of ELF files.
- Works similarly to `nm` and `readelf` tools.

### my_strace
- Trace and display all system calls made by a binary.
- Display arguments and return values for selected system calls.
- Optionally filter and sandbox specific system calls (bonus).

### my_db
- Debug a binary with basic commands like `continue`, `registers`, and `memdump`.
- Step-by-step execution and breakpoint management.
- Display a call stack.

### my_prof (Bonus)
- Count function calls within a binary.
- Generate a call graph in DOT format.
- Perform hotspot analysis for timing information.

---

## Requirements

- **Operating System**: Linux (tested with ASLR disabled and binaries compiled with `-static` flag)
- **Language**: C
- **Compiler**: `gcc`
- **Flags**: `-std=c99 -pedantic -Wall -Wextra -Wvla -Werror`

---

## Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   ```

2. Navigate to the project directory:
   ```bash
   cd my-debugging-suite
   ```

3. Build the tools:
   ```bash
   make all
   ```

---

## Usage

### my_nm
```bash
./my_nm /path/to/binary
```

### my_strace
```bash
./my_strace /path/to/binary
```

### my_db
```bash
./my_db /path/to/binary
```

### my_prof (Bonus)
```bash
./my_prof [-o output.txt | -d output.dot] /path/to/binary
```

---

## Authors

- Killian Heritier (<killian.heritier@epita.fr>)
- Paul Baudinot (<paul.baudinot@epita.fr>)

---

## License

This project is for internal use only within EPITA. Redistribution or sharing of this document is prohibited.

---

```                                      
                .::-=--:                                    
              -*%%@@@@@@#=. .::=++*+*+=-:                   
            :+%@@@@@@@@@@@%+#@@@@@@@@@@@@#*:                
          .+@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%=.              
         -#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=.             
        +@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%:            
      :*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#:          
      *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@=          
     -%@@@@@@@@@@@@@@@%%%@@@@@@@@@@@@@@@@@@@@@@@%=.         
    .*@@@@@@@@@@@@@@@=. .---====+====---::::+@@@=           
     .=#@@@@@@@@@@@@+                        #@@:           
        =%@@@@@@@@@@-                        =-:            
         .:#@@@@@@@@%*=:.        ....:--==+=-=-             
          :#@@@@@@@@@@@@%*+++: =%%%@@@@@@@@@%+@:            
          -@@@@@@@@@@@@@@@@@@+:+@@@@@@@@@@@@@%=.            
          .*@@@@@@@@@@@@@@@@@- .=#%%%%%%%##*=.              
           .-#@@@@@@@@@@@@@@@:    .::...                    
            .=@@@@@%#%@@@@@@%.     :=+-                     
        .:-+%@@@@@@@@@@@@@@@@#+-:-. .::.   :--. ...         
    .=+#%%@@@@@@@@@@@@@@@@@@%*%%++-:---  .+@@%+-=%#+.       
    :@@@%%@@@@@@@@@@@@@@@@@@@@%%=.     :*@@@@%##%%-@-       
     :*##@@@@@%@@@@@@@@@@@@@@#-.   ..-#@@@@@@%#=%@=-.       
       :%@%@@%#@@@@@@@@@@@%%%%%%#**%@@@@@@@@@@%:-*-         
        .-#@@#%@@@@@@@@@@@@@@@@@@@@@@@@@@@%@@@@=.           
          .-#*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*:.            
             .-+%@@@@@@@@@@@@@@@@@@@@@@@@#*-.               
                .:=*#%@@@@@@@@@@@@@@%#*=:.                  
                     .:--==++++==-::.                       
```
                        La rage de vaincre

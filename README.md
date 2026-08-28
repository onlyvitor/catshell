<img src="https://media1.tenor.com/m/Va_MYd9zdeUAAAAd/gato-mouse.gif" width="300">

# catshell — A shell for cat lovers

A simple, educational Unix shell written in C. Inspired by the book *Operating Systems: Design and Implementation* (exercise 28) and the tutorial "Write a Shell in C" by Stephen Brennan.

## Features

- **REPL loop** — Read, Evaluate, Print, Loop cycle
- **Built-in commands**: `echo`, `env`, `exit`
- **External command execution** — Runs any system program via `fork()` + `execvp()`
- **Colored prompt** — Shows current directory in cyan
- **ASCII art banner** — Cat-themed welcome screen
- **Memory safe** — Proper allocation/freeing with `free_args()`
- **Clean build system** — Makefile with dependency tracking

## Quick Start

```bash
make run
```

Or build and run separately:

```bash
make          # builds to build/bin/catshell
./build/bin/catshell
```

### Clean build

```bash
make clean && make
```

## Usage

Once running, you'll see the cat banner and a colored prompt:

```
 ██████╗ █████╗ ████████╗███████╗██╗  ██╗███████╗██╗     ██╗
 ...
[/home/user]$ 
```

### Built-in commands

| Command | Description |
|---------|-------------|
| `echo [args...]` | Prints arguments separated by spaces |
| `env` | Lists all environment variables |
| `exit` | Exits the shell |

### External commands

Any other command runs as a child process (e.g., `ls`, `pwd`, `cat`, `grep`, etc.).

### Exit the shell

- Type `exit`
- Press `Ctrl+D` (EOF)

## Project Structure

```
catshell/
├── catshell.c              # main() and REPL loop
├── catshell.h              # umbrella header (includes all modules)
├── Makefile                # build automation with dependency tracking
├── README.md               # this file
├── readline/
│   ├── cat_read_line.c/.h  # reads input, shows colored prompt with CWD
│   └── parser.c/.h         # splits input line into tokens (MAX_ARGS=100)
├── commands/
│   ├── commands.h          # builtin function declarations
│   ├── echo.c              # echo builtin
│   ├── env.c               # env builtin
│   └── exit.c              # exit builtin
└── utils/
    ├── exec.c/.h           # command dispatch (builtins + external via fork/execvp)
    ├── utils.c/.h          # get_current_directory(), free_args(), ANSI colors
    ├── free_args.c         # frees parsed token array
    └── arts/
        └── banner.c/.h     # ASCII art "CATSHELL" banner
```

## Architecture Overview

### The REPL Cycle (in `catshell.c`)

```c
while (MAGIC_NUMBER) {        // MAGIC_NUMBER = 0xCE77 ("cat" in hex)
    line = cat_read_line();   // 1. READ — shows prompt, reads line
    if (!line) break;         //    EOF (Ctrl+D) → exit loop
    args = parse_input(line); // 2. EVALUATE — tokenize into words
    exec_command(args);       // 3. EXECUTE — builtin or external
    free_args(args);          // 4. CLEANUP — free tokens
    free(line);
}
```

### Command Execution (`utils/exec.c`)

1. **Check for builtins** — Iterates `g_builtin[]` table (`echo`, `env`, `exit`)
2. **If not found** — Prints "command not found", then tries external execution
3. **External execution** — `fork()` → child runs `execvp()`, parent `wait()`s

### Key Constants

- `MAGIC_NUMBER` = `0xCE77` — keeps the REPL loop running (spells "cat" in leet)
- `MAX_ARGS` = 100 — maximum tokens per command line

## Key Concepts

| Concept | Description |
|---------|-------------|
| **REPL** | Read-Evaluate-Print-Loop — the fundamental shell cycle |
| **Prompt** | The `[cwd]$ ` text inviting user input |
| **Builtin** | Command implemented inside the shell (no new process) |
| **External command** | System program executed in a child process |
| **fork()** | Creates a child process (copy of parent) |
| **execvp()** | Replaces child process image with new program |
| **wait()** | Parent pauses until child terminates |
| **getcwd()** | Retrieves current working directory |
| **environ** | Global array of environment variables |
| **Token** | Individual word/argument from parsed input |
| **EOF (Ctrl+D)** | End-of-file signal to exit the shell |

## Sources & References

- [Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/) — Stephen Brennan's tutorial
- [Writing a Unix Shell](https://igupta.in/blog/writing-a-unix-shell-part-1/) — by I. Gupta
- [The Architecture of Open Source Applications: Bash](https://aosabook.org/en/v1/bash.html)
- *Operating Systems: Design and Implementation* — Tanenbaum & Woodhull (exercise 28)

## License

BSD 3-Clause License — see [LICENSE](LICENSE) for details.
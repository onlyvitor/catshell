<img src="https://media1.tenor.com/m/Va_MYd9zdeUAAAAd/gato-mouse.gif" width="300">

# catshell — A shell for cat lovers

A simple, educational Unix shell written in modern C++ (C++20). Inspired by the book *Operating Systems: Design and Implementation* (exercise 28) and the tutorial "Write a Shell in C" by Stephen Brennan.

## Features

- **REPL loop** — Read, Evaluate, Print, Loop cycle
- **Built-in commands**: `echo`, `env`, `exit`
- **External command execution** — Runs any system program via `fork()` + `execvp()`
- **Pipelines** — Chain commands with `|` (e.g. `ls | head -2`)
- **Colored prompt** — Shows current directory in cyan
- **ASCII art banner** — Cat-themed welcome screen
- **Memory safe** — RAII and `std::vector`/`std::string`; no manual allocation
- **Clean build system** — Makefile with dependency tracking
- idk

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

### Pipelines

Commands can be chained with `|` (up to `MaxPipes = 10` stages):

```
[/home/user]$ ls | head -2
```

### Exit the shell

- Type `exit`
- Press `Ctrl+D` (EOF)

## Project Structure

```
catshell/
├── catshell.cpp             # main() and REPL loop
├── Makefile                 # build automation with dependency tracking
├── README.md                # this file
├── readline/
│   ├── reader.cpp/.hpp      # reads input, shows colored prompt with CWD
│   └── parser.cpp/.hpp      # splits input into Command/Pipeline (MaxPipes=10)
├── commands/
│   ├── commands.hpp         # builtin declarations + Args type
│   ├── echo.cpp             # echo builtin
│   ├── env.cpp              # env builtin
│   └── exit.cpp             # exit builtin
└── utils/
    ├── exec.cpp/.hpp        # command dispatch + RAII Pipe/Child process classes
    ├── utils.cpp/.hpp       # current_directory() via std::filesystem
    ├── colors.hpp           # ANSI colors (single source of truth)
    └── arts/
        └── banner.cpp/.hpp  # ASCII art "CATSHELL" banner
```

## Architecture Overview

### The REPL Cycle (in `catshell.cpp`)

```cpp
while (true) {
    auto line = catshell::read_line();   // 1. READ — prompt + std::getline
    if (!line) break;                    //    EOF (Ctrl+D) → exit loop
    Pipeline pipeline = parse_input(*line); // 2. EVALUATE — tokenize into commands
    exec_pipeline(pipeline);             // 3. EXECUTE — builtin or external
}                                        // 4. CLEANUP — RAII, nothing to free
```

### Command Execution (`utils/exec.cpp`)

1. **Builtin registry** — A table of `{name, function}` pairs (`echo`, `env`, `exit`)
2. **If not found** — External execution in a child process
3. **RAII primitives** — `Pipe` owns a pipe fd pair, `Child` owns a forked pid and reaps it on destruction
4. **Pipelines** — One `Child` per command, daisy-chained through `Pipe` objects; fds close automatically

### Key Constants

- `MaxPipes` = 10 — maximum commands per pipeline

## Key Concepts

| Concept | Description |
|---------|-------------|
| **REPL** | Read-Evaluate-Print-Loop — the fundamental shell cycle |
| **Prompt** | The `[cwd]$ ` text inviting user input |
| **Builtin** | Command implemented inside the shell (no new process) |
| **External command** | System program executed in a child process |
| **fork()** | Creates a child process (copy of parent) |
| **execvp()** | Replaces child process image with new program |
| **waitpid()** | Parent pauses until child terminates |
| **RAII** | Resource Acquisition Is Initialization — fds/pids freed by destructors |
| **std::optional** | `read_line()` returns `nullopt` on EOF instead of a sentinel |
| **std::filesystem** | Portable current working directory retrieval |
| **std::istringstream** | Whitespace tokenization and `|` segment splitting |

## Sources & References

- [Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/) — Stephen Brennan's tutorial
- [Writing a Unix Shell](https://igupta.in/blog/writing-a-unix-shell-part-1/) — by I. Gupta
- [The Architecture of Open Source Applications: Bash](https://aosabook.org/en/v1/bash.html)
- *Operating Systems: Design and Implementation* — Tanenbaum & Woodhull (exercise 28)

## License

BSD 3-Clause License — see [LICENSE](LICENSE) for details.

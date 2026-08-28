# THIS_PROJECT.md — catshell

Um guia completo e simples de como o **catshell** funciona, arquivo por arquivo e linha por linha.

---

## Visão geral

O **catshell** é um shell (interpretador de comandos) escrito em **C**. Ele é um projeto de estudo
inspirado no livro *Operating Systems: Design and Implementation* e no tutorial "Write a Shell in C".

Um shell é um programa que lê comandos que o usuário digita, interpreta e executa. É aquele programa
que mostra um "prompt" (o texto onde você digita) e espera o seu comando — como o `bash` do Linux,
mas bem mais simples.

O tema é "gato": o logo é uma arte ASCII, o prompt é colorido e a constante mágica do loop é `0xCE77`
(lembra a palavra "cat").

### Como o programa flui (o ciclo REPL)

Todo shell funciona em um ciclo chamado **REPL**:

1. **R**ead — lê o que o usuário digitou
2. **E**valuate — interpreta / entende o comando
3. **P**rint — mostra o resultado
4. **L**oop — volta para o início e espera o próximo comando

O `catshell` segue exatamente esse fluxo dentro da função `main`.

---

## Estrutura do projeto

```
catshell/
├── catshell.c                 # função main() e o ciclo REPL
├── catshell.h                 # cabeçalho "guarda-chuva" (junta tudo)
├── Makefile                   # automatiza a compilação
├── README.md                  # descrição resumida
├── readline/
│   ├── cat_read_line.c/.h     # lê a linha digitada + mostra o prompt
│   └── parser.c/.h            # divide a linha em "pedaços" (tokens)
├── commands/
│   ├── commands.h             # declarações dos comandos internos
│   ├── echo.c                 # comando echo
│   ├── env.c                  # comando env
│   └── exit.c                 # comando exit
└── utils/
    ├── exec.c/.h              # executa comandos (internos e externos)
    ├── utils.c/.h             # função auxiliar de diretório atual
    └── arts/
        ├── banner.c/.h        # desenha o logo ASCII
```

---

## catshell.c — o coração do programa

Este arquivo tem a função `main()`, que é por onde o programa começa.

```c
#include "catshell.h"
```

Inclui o cabeçalho que já junta todas as outras partes do projeto.

```c
int main(int ac, char **av) {
    char *line;
    char **args;
```

- `ac` e `av` são os argumentos da linha de comando (não usados aqui).
- `line` vai guardar o texto que o usuário digitou.
- `args` vai guardar o texto "quebrado" em palavras.

```c
    print_banner();
```

Chama a função que desenha o logo ASCII azul na tela.

```c
    while(MAGIC_NUMBER){
```

`MAGIC_NUMBER` é definido em `catshell.h` como `0xCE77`. Como é um número diferente de zero,
o `while` é sempre verdadeiro → isso cria um **loop infinito**, ou seja, o shell roda para sempre.
Ele só sai quando o comando `exit` é dado (que chama `exit(0)` e termina o processo).

```c
        line = cat_read_line();
        if (line == NULL) {
            break;
        }
```

- Lê uma linha do teclado (mostrando o prompt colorido antes).
- Se o usuário apertar **Ctrl+D** (fim de arquivo / EOF), a função devolve `NULL` e o loop é
  interrompido (`break`) — é assim que se sai do shell digitando Ctrl+D.

```c
        args = parse_input(line);
```

Divide a linha lida em palavras separadas (tokens). Ex.: `"echo oi"` vira `["echo", "oi", NULL]`.

```c
        exec_command(args);
```

Executa o comando. Se for um comando interno (builtin) executa na hora; senão, cria um processo
novo para rodar o programa externo.

```c
    }
    return EXIT_SUCCESS;
}
```

Quando o loop acaba (Ctrl+D), retorna sucesso e encerra o programa.

---

## catshell.h — o cabeçalho "guarda-chuva"

Este cabeçalho apenas junta os outros arquivos do projeto para que a `main` consiga usá-los.

```c
#ifndef CATSHELL_H
#define CATSHELL_H

#define MAGIC_NUMBER 0xCE77
```

- `#ifndef` / `#define` / `#endif` é o **guard de inclusão**: evita que o arquivo seja incluído
  duas vezes por engano.
- `MAGIC_NUMBER` é a constante que mantém o loop infinito rodando.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
```

Inclui as bibliotecas padrão do C usadas no projeto.

```c
#include "readline/cat_read_line.h"
#include "utils/utils.h"
#include "readline/parser.h"
#include "utils/exec.h"
```

Inclui os cabeçalhos de cada parte do shell.

---

## readline/cat_read_line.c — lendo a linha e mostrando o prompt

Responsável por mostrar o prompt colorido e ler o que o usuário digita.

```c
char *cat_read_line(void) {
    char *buf = NULL;
    size_t buf_size = 0;
    char cwd[BUFSIZ];
```

- `buf` guarda o texto digitado (começa `NULL`).
- `buf_size` guarda o tamanho da memória alocada (começa `0`; o `getline` faz a alocação sozinho).
- `cwd` é um espaço para guardar o diretório atual.

```c
    char *current_dir = get_current_directory(cwd, sizeof(cwd));
```

Pega o diretório atual de trabalho (ex.: `/home/user`) usando a função auxiliar de `utils.c`.

```c
    printf(ANSI_COLOR_CYAN"[%s]$ "RESET, current_dir);
```

Mostra o **prompt**: o diretório atual em ciano, seguido de `$ `. O `ANSI_COLOR_CYAN` e o `RESET`
são códigos de cor do terminal. É a "pergunta" que o shell faz para o usuário digitar algo.

```c
    if (getline(&buf, &buf_size, stdin) == -1) {
        buf = NULL;

        if (feof(stdin)) {
            printf(ANSI_COLOR_BLUE"[EOF]\n"RESET);
        }
        else{
            printf(ANSI_COLOR_BLUE" a Error ocurred\n"RESET);
        }
    }
```

- `getline` lê uma linha inteira do terminal (`stdin`).
- Se ele devolve `-1`, algo deu errado ou acabou a entrada:
  - `feof` diz se acabou a entrada (usuário apertou Ctrl+D) → imprime `[EOF]` em azul.
  - Caso contrário, algum erro de leitura aconteceu.
- O `buf` é setado para `NULL` nesses casos, para a `main` saber que deve parar.

```c
    else {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }
    }
    return buf;
}
```

- Se a leitura funcionou, o `getline` inclui um `\n` no final (a tecla Enter).
- Esse `\n` é trocado por `\0` (fim de string) para o texto ficar limpo.
- Por fim, devolve a linha lida.

---

## readline/parser.c — quebrando a linha em palavras

Transforma a frase digitada em uma lista de palavras separadas (tokens). Ex.: `"echo oi mundo"`
vira `["echo", "oi", "mundo", NULL]`.

```c
char** parse_input(char* input) {
    char** tokens = malloc((MAX_ARGS + 1) * sizeof(char*));
    size_t token_position = 0;
```

- Aloca memória para guardar até `MAX_ARGS` (definido como 100) ponteiros, mais 1 para o NULL final.
- `token_position` conta quantas palavras já foram guardadas.

```c
    if (tokens == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
```

Se a memória não foi alocada (erro), mostra o erro e encerra o programa.

```c
    char* token = strtok(input, " ");
    while (token != NULL) {
```

- `strtok` divide a string usando o espaço como separador e devolve uma palavra por vez.
- O `while` continua enquanto houver palavras (até `strtok` devolver `NULL`).

```c
        if (token_position >= MAX_ARGS) {
            fprintf(stderr, "parse_input: too many arguments (max %d)\n", MAX_ARGS);
            break;
        }
```

Se o número de palavras passou do limite, avisa e para (impede estouro de memória).

```c
        tokens[token_position] = malloc(strlen(token) + 1);
        if (tokens[token_position] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
```

Aloca memória do tamanho certo para guardar cada palavra (o `+1` é para o `\0` final) e verifica erro.

```c
        strcpy(tokens[token_position], token);
        token_position++;
        token = strtok(NULL, " ");
    }
```

- Copia a palavra para o array (`strcpy`).
- Aumenta a contagem e pede a próxima palavra ao `strtok`.
- **Detalhe:** o `strtok` ignora os espaços extras, então espaços seguidos ou no início não criam
  palavras vazias (o que evitava um bug de crash antes).

```c
    tokens[token_position] = NULL;
    return tokens;
}
```

Marca o fim da lista com `NULL` (jeito padrão de mostrar que a lista acabou) e devolve.

---

## utils/exec.c — executando comandos

A função mais importante: decide o que fazer com o que o usuário digitou.

### A tabela de comandos internos (builtins)

```c
builtin_t g_builtin[] = {
    {.name="echo", .func=echo_func},
    {.name="env", .func=env_func},
    {.name="exit", .func=exit_func},
    {.name="", .func=NULL}
};
```

- Aqui está a lista de comandos **internos** (implementados dentro do próprio shell).
- Cada entrada tem um `name` (texto) e um `func` (função C que executa).
- A última entrada, com nome vazio `""`, marca o **fim da tabela** — o loop sabe que deve parar.

### A função `exec_command` — decidir o que fazer

```c
void exec_command(char **args) {
    if (args[0] == NULL) {
        return;
    }
```

Se a linha estava vazia (usuário só apertou Enter), `args[0]` é `NULL`. Nesse caso, simplesmente
volta sem fazer nada. **Isso impede um crash** (antes, o programa tentava comparar um `NULL`).

```c
    for (size_t i = 0; g_builtin[i].name[0] != '\0'; i++) {
        if (strcmp(args[0], g_builtin[i].name) == 0) {
            g_builtin[i].func(args);
            return;
        }
    }
```

- Percorre a tabela de builtins.
- Compara a primeira palavra digitada (`args[0]`) com cada nome da tabela.
- Se achar, chama a função correspondente (`g_builtin[i].func(args)`) e volta.
- O `return` garante que só executa uma vez. A condição `name[0] != '\0'` para o loop no fim da
  tabela (<> leitura além do limite).

```c
    printf("%s: command not found\n", args[0]);
    cat_launch(args);
}
```

- Se não era um builtin, imprime "command not found" **uma vez**.
- Depois tenta executar como **comando externo** (um programa real do sistema).

### A função `cat_launch` — rodar programas externos

```c
void cat_launch(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // child
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        // parent
        wait(NULL);
    }
}
```

- **`fork()`** cria uma cópia (um "clone") do processo atual. Depois do `fork` existem dois
  processos: o **pai** (original) e o **filho** (cópia).
  - No filho, `pid == 0`.
  - No pai, `pid` é o número do processo filho.
  - Se `pid < 0`, deu erro ao criar o filho.
- **No filho** (`pid == 0`): `execvp` **substitui** o código do filho pelo programa que o usuário
  pediu (ex.: `ls`). Se der erro, mostra e termina.
- **No pai**: `wait(NULL)` **espera** o filho terminar antes de mostrar o próximo prompt. Assim o
  shell não fica "dando tchau" antes da hora.

---

## utils/utils.c — função auxiliar de diretório

```c
char *get_current_directory(char *buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        perror(ANSI_COLOR_RED "get_current_directory: buffer is NULL or size is 0" RESET);
        return NULL;
    }
```

Verifica se os argumentos são válidos (o buffer existe e tem tamanho). Se não, mostra erro e volta.

```c
    if (getcwd(buffer, size) == NULL) {
        perror(ANSI_COLOR_RED "getcwd" RESET);
        return NULL;
    }

    return buffer;
}
```

- `getcwd` é uma função do sistema que pega o **diretório atual de trabalho** (o caminho da pasta
  onde o shell está rodando) e guarda no buffer.
- Se der erro, mostra e volta `NULL`. Senão, devolve o buffer com o caminho.

---

## utils/arts/banner.c — o logo ASCII

```c
void print_banner(void) {
    printf(ANSI_COLOR_BLUE
           " ██████╗ █████╗ ████████╗███████╗██╗  ██╗███████╗██╗     ██╗\n"
           ...
           RESET);
}
```

Uma única função que imprime a palavra "CATSHELL" desenhada com caracteres de bloco (ASCII art),
toda em azul. O `ANSI_COLOR_BLUE` colore e o `RESET` volta à cor normal. É só decoração de boas-vindas.

---

## commands/ — os comandos internos (builtins)

### echo.c — imprimir texto

```c
int echo_func(char **args) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 0;
}
```

- Começa em `i = 1` para pular o próprio nome ("echo") em `args[0]`.
- Imprime cada palavra (com um espaço no fim) e depois uma quebra de linha.
- Ex.: `echo oi mundo` mostra `oi mundo`.

### env.c — mostrar variáveis de ambiente

```c
int env_func(char **args) {
    extern char **environ;
    if (environ == NULL) {
        return 1;
    }
    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
    return 0;
}
```

- `environ` é uma lista global com as **variáveis de ambiente** do sistema (como `PATH`, `HOME`).
- Imprime cada uma em uma linha.
- Existe em todo processo Linux, por isso `extern` (de fora do arquivo).

### exit.c — sair do shell

```c
int exit_func(char **args) {
    (void)args;
    exit(0);
}
```

- `(void)args` só diz "não vou usar esse parâmetro" (evita aviso do compilador).
- `exit(0)` termina o programa imediatamente com código de sucesso.

### commands.h — declarações

```c
int echo_func(char **);
int exit_func(char **);
int env_func(char **);
```

Só declara (anuncia) as funções dos comandos para que outros arquivos possam usá-las.

---

## utils/exec.h — a estrutura do builtin

```c
typedef struct s_builtin {
    char *name;
    int (*func)(char **args);
} builtin_t;
```

Define uma "ficha" que junta o nome do comando (`name`) com a sua função (`func`, um ponteiro para
função). É isso que a tabela `g_builtin` usa. Inclui também `<sys/wait.h>` (para o `wait`) e
`<unistd.h>` (para `fork`/`execvp`).

---

## Makefile — como o projeto é compilado

```make
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -pedantic -g
```

- `CC` é o compilador (gcc).
- `CFLAGS` são as opções: `-std=c11` (usa o padrão C de 2011), `-Wall -Wextra -pedantic` (mostra
  muitos avisos úteis) e `-g` (guarda informações de debug).

```make
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj
```

Define as pastas de saída: os `.o` (objetos) e o executável ficam dentro de `build/`.

```make
SRCS := catshell.c readline/cat_read_line.c ... commands/env.c
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))
```

- `SRCS` lista todos os arquivos `.c` do projeto.
- `OBJS` transforma cada `.c` em um `.o` correspondente dentro de `build/obj/`.

```make
all: dirs $(TARGET_BIN)

dirs:
	@mkdir -p $(BIN_DIR) $(dir $(OBJS))
```

`all` é o alvo padrão: cria as pastas e compila. `dirs` garante que as pastas existam.

```make
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
```

Regra para compilar cada `.c` em `.o`. `-MMD -MP` gera arquivos `.d` (de dependências) — assim, se
um cabeçalho mudar, o Makefile sabe que precisa recompilar.

```make
$(TARGET_BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

run: all
	./$(TARGET_BIN)
```

- Liga todos os `.o` no executável final chamado `catshell`.
- `run` compila e roda o shell.

```make
clean:
	rm -rf $(BUILD_DIR)
```

`make clean` apaga a pasta `build/` (recomeça do zero).

---

## Conceitos-chave (explicados de forma simples)

| Conceito | O que é |
|----------|---------|
| **REPL** | Read-Evaluate-Print-Loop: ler, interpretar, mostrar e repetir. É o ciclo do shell. |
| **Prompt** | O texto que o shell mostra para você digitar (o `[caminho]$ ` colorido). |
| **Builtin** | Comando que o próprio shell executa (echo, env, exit). |
| **Comando externo** | Um programa de verdade do sistema (ls, pwd, cat...) rodado num processo novo. |
| **fork()** | Copia o processo atual. Depois dele há o pai e o filho (dois "cópias"). |
| **execvp()** | Troca o código do processo filho pelo programa que será executado. |
| **wait()** | O pai espera o filho terminar. |
| **getcwd()** | Descobre a pasta atual de trabalho. |
| **environ** | Lista de variáveis de ambiente do sistema. |
| **Token** | Cada palavra separada de um comando. |
| **EOF (Ctrl+D)** | Fim da entrada do teclado — o sinal para o shell sair. |

---

# English Version

## Overview

**catshell** is a shell (command interpreter) written in **C**. It is a study project inspired by
the book *Operating Systems: Design and Implementation* and by the tutrial "Write a Shell in C".

A shell is a program that reads what the user types, interprets it, and runs it. It is that program
that shows a "prompt" (the text where you type) and waits for your command — like `bash` on Linux,
but much simpler.

The theme is "cat": the logo is ASCII art, the prompt is colorful, and the magic loop constant is
`0xCE77`.

### The REPL cycle

Every shell works in a loop called **REPL**:

1. **R**ead — read what the user typed
2. **E**valuate — interpret the command
3. **P**rint — show the result
4. **L**oop — go back and wait for the next command

`catshell` follows exactly this flow inside the `main` function.

## Project structure

```
catshell/
├── catshell.c                 # main() function and the REPL loop
├── catshell.h                 # "umbrella" header (joins everything)
├── Makefile                   # automates compilation
├── README.md                  # short description
├── readline/
│   ├── cat_read_line.c/.h     # reads the typed line + shows the prompt
│   └── parser.c/.h            # splits the line into tokens
├── commands/
│   ├── commands.h             # declarations of the internal commands
│   ├── echo.c                 # echo command
│   ├── env.c                  # env command
│   └── exit.c                 # exit command
└── utils/
    ├── exec.c/.h              # runs commands (internal and external)
    ├── utils.c/.h             # helper function for current directory
    └── arts/
        ├── banner.c/.h        # draws the ASCII logo
```

## catshell.c — the heart of the program

This file has the `main()` function, where the program starts.

```c
#include "catshell.h"
```
Includes the header that already joins all the other parts of the project.

```c
int main(int ac, char **av) {
    char *line;
    char **args;
```
- `ac` and `av` are the command-line arguments (not used here).
- `line` will hold the text the user typed.
- `args` will hold the text "broken" into words.

```c
    print_banner();
```
Calls the function that draws the blue ASCII logo.

```c
    while(MAGIC_NUMBER){
```
`MAGIC_NUMBER` is defined in `catshell.h` as `0xCE77`. Since it is non-zero, the loop is always
true → this creates an **infinite loop**, so the shell runs forever. It only stops when `exit` is
typed (which calls `exit(0)` and ends the process).

```c
        line = cat_read_line();
        if (line == NULL) {
            break;
        }
```
- Reads a line from the keyboard (showing the colored prompt first).
- If the user presses **Ctrl+D** (EOF), the function returns `NULL` and the loop is stopped with
  `break` — that's how you exit the shell with Ctrl+D.

```c
        args = parse_input(line);
```
Splits the read line into separate words (tokens). E.g. `"echo oi"` becomes `["echo", "oi", NULL]`.

```c
        exec_command(args);
```
Runs the command. If it's a builtin, it runs immediately; otherwise it creates a new process to
run the external program.

```c
    }
    return EXIT_SUCCESS;
}
```
When the loop ends (Ctrl+D), returns success and finishes the program.

## catshell.h — the "umbrella" header

```c
#ifndef CATSHELL_H
#define CATSHELL_H

#define MAGIC_NUMBER 0xCE77
```
- `#ifndef` / `#define` / `#endif` is the **include guard**: it prevents the file from being
  included twice by mistake.
- `MAGIC_NUMBER` is the constant that keeps the infinite loop running.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
```
Include the standard C libraries used in the project.

```c
#include "readline/cat_read_line.h"
#include "utils/utils.h"
#include "readline/parser.h"
#include "utils/exec.h"
```
Include the headers of each part of the shell.

## readline/cat_read_line.c — reading the line and showing the prompt

```c
char *cat_read_line(void) {
    char *buf = NULL;
    size_t buf_size = 0;
    char cwd[BUFSIZ];
```
- `buf` holds the typed text (starts as `NULL`).
- `buf_size` holds the allocated memory size (starts at `0`; `getline` handles allocation itself).
- `cwd` is a space to store the current directory.

```c
    char *current_dir = get_current_directory(cwd, sizeof(cwd));
```
Gets the current working directory using the helper in `utils.c`.

```c
    printf(ANSI_COLOR_CYAN"[%s]$ "RESET, current_dir);
```
Shows the **prompt**: the current directory in cyan, then `$ `. `ANSI_COLOR_CYAN` and `RESET` are
terminal color codes.

```c
    if (getline(&buf, &buf_size, stdin) == -1) {
        buf = NULL;
        if (feof(stdin)) {
            printf(ANSI_COLOR_BLUE"[EOF]\n"RESET);
        }
        else{
            printf(ANSI_COLOR_BLUE" a Error ocurred\n"RESET);
        }
    }
```
- `getline` reads a whole line from the terminal (`stdin`).
- If it returns `-1`, something failed or input ended:
  - `feof` tells if input ended (Ctrl+D) → prints `[EOF]` in blue.
  - Otherwise, some read error happened.
- `buf` is set to `NULL` so `main` knows to stop.

```c
    else {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }
    }
    return buf;
}
```
- If reading worked, `getline` includes a `\n` at the end (the Enter key).
- That `\n` is replaced with `\0` (end of string) to clean up the text.
- Finally returns the read line.

## readline/parser.c — splitting the line into words

```c
char** parse_input(char* input) {
    char** tokens = malloc((MAX_ARGS + 1) * sizeof(char*));
    size_t token_position = 0;
```
- Allocates memory to hold up to `MAX_ARGS` (defined as 100) pointers, plus 1 for the final NULL.
- `token_position` counts how many words were stored.

```c
    if (tokens == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
```
If memory allocation failed, show the error and exit.

```c
    char* token = strtok(input, " ");
    while (token != NULL) {
```
- `strtok` splits the string using space as the separator and returns one word at a time.
- The `while` continues while there are words.

```c
        if (token_position >= MAX_ARGS) {
            fprintf(stderr, "parse_input: too many arguments (max %d)\n", MAX_ARGS);
            break;
        }
```
If the word count passed the limit, warn and stop (prevents memory overflow).

```c
        tokens[token_position] = malloc(strlen(token) + 1);
        if (tokens[token_position] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
```
Allocates exactly enough memory for each word (the `+1` is for the trailing `\0`).

```c
        strcpy(tokens[token_position], token);
        token_position++;
        token = strtok(NULL, " ");
    }
```
- Copies the word into the array (`strcpy`).
- Increases the count and asks `strtok` for the next word.
- **Note:** `strtok` ignores extra spaces, so consecutive or leading spaces don't create empty
  words (this avoided a crash bug).

```c
    tokens[token_position] = NULL;
    return tokens;
}
```
Marks the end of the list with `NULL` (the standard way to show a list ended) and returns it.

## utils/exec.c — running commands

### The builtins table

```c
builtin_t g_builtin[] = {
    {.name="echo", .func=echo_func},
    {.name="env", .func=env_func},
    {.name="exit", .func=exit_func},
    {.name="", .func=NULL}
};
```
- This is the list of **internal** commands (implemented inside the shell).
- Each entry has a `name` (text) and a `func` (C function that runs it).
- The last entry, with an empty name `""`, marks the **end of the table** — the loop knows to stop.

### `exec_command` — decide what to do

```c
void exec_command(char **args) {
    if (args[0] == NULL) {
        return;
    }
```
If the line was empty (user just pressed Enter), `args[0]` is `NULL`. Then simply return without
doing anything. **This prevents a crash** (before, the program tried to compare a `NULL`).

```c
    for (size_t i = 0; g_builtin[i].name[0] != '\0'; i++) {
        if (strcmp(args[0], g_builtin[i].name) == 0) {
            g_builtin[i].func(args);
            return;
        }
    }
```
- Iterates over the builtin table.
- Compares the first typed word (`args[0]`) with each table name.
- If found, calls the corresponding function and returns.
- The `return` guarantees it runs only once. The condition `name[0] != '\0'` stops the loop at the
  end of the table (no reading past the limit).

```c
    printf("%s: command not found\n", args[0]);
    cat_launch(args);
}
```
- If it wasn't a builtin, prints "command not found" **once**.
- Then tries to run it as an **external command** (a real system program).

### `cat_launch` — run external programs

```c
void cat_launch(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // child
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        // parent
        wait(NULL);
    }
}
```
- **`fork()`** creates a copy ("clone") of the current process. After the fork there are two
  processes: the **parent** (original) and the **child** (copy).
  - In the child, `pid == 0`.
  - In the parent, `pid` is the child's process number.
  - If `pid < 0`, creating the child failed.
- **In the child** (`pid == 0`): `execvp` **replaces** the child's code with the program the user
  asked for (e.g. `ls`). If it fails, show the error and exit.
- **In the parent**: `wait(NULL)` **waits** for the child to finish before showing the next prompt.

## utils/utils.c — directory helper function

```c
char *get_current_directory(char *buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        perror(ANSI_COLOR_RED "get_current_directory: buffer is NULL or size is 0" RESET);
        return NULL;
    }
```
Checks if the arguments are valid. If not, shows an error and returns.

```c
    if (getcwd(buffer, size) == NULL) {
        perror(ANSI_COLOR_RED "getcwd" RESET);
        return NULL;
    }

    return buffer;
}
```
- `getcwd` is a system function that gets the **current working directory** (the path of the folder
  where the shell is running) and stores it in the buffer.
- On error it returns `NULL`, otherwise it returns the buffer with the path.

## utils/arts/banner.c — the ASCII logo

```c
void print_banner(void) {
    printf(ANSI_COLOR_BLUE
           " ██████╗ █████╗ ████████╗███████╗██╗  ██╗███████╗██╗     ██╗\n"
           ...
           RESET);
}
```
A single function that prints the word "CATSHELL" drawn with block characters (ASCII art), all in
blue. `ANSI_COLOR_BLUE` colors it and `RESET` returns to normal.

## commands/ — the internal commands (builtins)

### echo.c — print text

```c
int echo_func(char **args) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 0;
}
```
- Starts at `i = 1` to skip the command name itself ("echo") in `args[0]`.
- Prints each word (with a space). E.g. `echo oi mundo` shows `oi mundo`.

### env.c — show environment variables

```c
int env_func(char **args) {
    extern char **environ;
    if (environ == NULL) {
        return 1;
    }
    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
    return 0;
}
```
- `environ` is a global list with the system's **environment variables** (like `PATH`, `HOME`).
- Prints each one on its own line.

### exit.c — exit the shell

```c
int exit_func(char **args) {
    (void)args;
    exit(0);
}
```
- `(void)args` just says "I won't use this parameter" (avoids a compiler warning).
- `exit(0)` ends the program immediately with a success code.

### commands.h — declarations

```c
int echo_func(char **);
int exit_func(char **);
int env_func(char **);
```
Only declares the command functions so other files can use them.

## utils/exec.h — the builtin structure

```c
typedef struct s_builtin {
    char *name;
    int (*func)(char **args);
} builtin_t;
```
Defines a "card" that joins the command name (`name`) with its function (`func`, a function
pointer). This is what the `g_builtin` table uses. It also includes `<sys/wait.h>` (for `wait`) and
`<unistd.h>` (for `fork`/`execvp`).

## Makefile — how the project is compiled

```make
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -pedantic -g
```
- `CC` is the compiler (gcc).
- `CFLAGS` are the options: `-std=c11` (uses the 2011 C standard), `-Wall -Wextra -pedantic` (shows
  many useful warnings) and `-g` (keeps debug info).

```make
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj
```
Defines the output folders: the `.o` objects and the executable live inside `build/`.

```make
SRCS := catshell.c readline/cat_read_line.c ... commands/env.c
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))
```
- `SRCS` lists all the `.c` files in the project.
- `OBJS` turns each `.c` into a corresponding `.o` inside `build/obj/`.

```make
all: dirs $(TARGET_BIN)

dirs:
	@mkdir -p $(BIN_DIR) $(dir $(OBJS))
```
`all` is the default target: it creates the folders and compiles. `dirs` makes sure the folders exist.

```make
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
```
Rule to compile each `.c` into a `.o`. `-MMD -MP` generates `.d` (dependency) files — so when a
header changes, the Makefile knows it must recompile.

```make
$(TARGET_BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

run: all
	./$(TARGET_BIN)
```
- Links all the `.o` into the final executable called `catshell`.
- `run` compiles and runs the shell.

```make
clean:
	rm -rf $(BUILD_DIR)
```
`make clean` deletes the `build/` folder (fresh start).

## Key concepts (simply explained)

| Concept | What it is |
|---------|------------|
| **REPL** | Read-Evaluate-Print-Loop: read, interpret, show, repeat. It's the shell's cycle. |
| **Prompt** | The text the shell shows for you to type (the colored `[path]$ `). |
| **Builtin** | A command the shell runs itself (echo, env, exit). |
| **External command** | A real system program (ls, pwd, cat...) run in a new process. |
| **fork()** | Copies the current process. After it there are parent and child ("copies"). |
| **execvp()** | Replaces the child's code with the program to be executed. |
| **wait()** | The parent waits for the child to finish. |
| **getcwd()** | Finds out the current working folder. |
| **environ** | List of the system's environment variables. |
| **Token** | Each separate word of a command. |
| **EOF (Ctrl+D)** | End of keyboard input — the signal for the shell to exit. |

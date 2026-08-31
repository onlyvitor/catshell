#define _POSIX_C_SOURCE 200809L
#include "exec.h"
#include <signal.h>


builtin_t g_builtin[] = {
    {.name="echo", .func=echo_func},
    {.name="env", .func=env_func},
    {.name="exit", .func=exit_func},
    {.name="", .func=NULL}
};

int is_builtin(char **args) {
    if (args[0] == NULL) return 0;
    for (size_t i = 0; g_builtin[i].name[0] != '\0'; i++) {
        if (strcmp(args[0], g_builtin[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

void exec_command(char **args) {
    if (args[0] == NULL) {
        return;
    }

    for (size_t i = 0; g_builtin[i].name[0] != '\0'; i++) {
        if (strcmp(args[0], g_builtin[i].name) == 0) {
            g_builtin[i].func(args);
            fflush(stdout);
            return;
        }
    }

    cat_launch(args);
}

void cat_launch(char **args) {
    fflush(stdout);
    fflush(stderr);
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        wait(NULL);
    }
}

static void execute_single_command(char **args, int in_fd, int out_fd) {
    if (in_fd != STDIN_FILENO) {
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }
    if (out_fd != STDOUT_FILENO) {
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }

    if (is_builtin(args)) {
        for (size_t i = 0; g_builtin[i].name[0] != '\0'; i++) {
            if (strcmp(args[0], g_builtin[i].name) == 0) {
                // `exit` must not call exit() in a pipeline child - use _exit instead
                if (strcmp(args[0], "exit") == 0) {
                    _exit(EXIT_SUCCESS);
                }
                g_builtin[i].func(args);
                fflush(stdout);
                fflush(stderr);
                _exit(EXIT_SUCCESS);
            }
        }
    } else {
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        _exit(EXIT_FAILURE);
    }
}

void exec_pipeline(pipeline_t* pipeline) {
    if (pipeline == NULL || pipeline->num_commands == 0) {
        return;
    }

    if (pipeline->num_commands == 1) {
        exec_command(pipeline->commands[0]);
        return;
    }

    int num_pipes = pipeline->num_commands - 1;
    int pipes[num_pipes][2];

    // Flush stdio buffers before fork to avoid duplicate output
    fflush(stdout);
    fflush(stderr);

    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            for (int j = 0; j < i; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return;
        }
    }

    pid_t pids[pipeline->num_commands];

    for (int i = 0; i < pipeline->num_commands; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            for (int j = 0; j < num_pipes; j++) {
                if (j == i - 1) {
                    close(pipes[j][1]);
                } else if (j == i) {
                    close(pipes[j][0]);
                } else {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }

            int in_fd = (i > 0) ? pipes[i - 1][0] : STDIN_FILENO;
            int out_fd = (i < num_pipes) ? pipes[i][1] : STDOUT_FILENO;

            execute_single_command(pipeline->commands[i], in_fd, out_fd);
        } else if (pids[i] < 0) {
            perror("fork");
            for (int j = 0; j <= i; j++) {
                if (pids[j] > 0) {
                    kill(pids[j], SIGKILL);
                }
            }
            for (int j = 0; j < num_pipes; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return;
        }
    }

    for (int i = 0; i < num_pipes; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < pipeline->num_commands; i++) {
        waitpid(pids[i], NULL, 0);
    }
}
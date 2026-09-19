#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "exec.h"
#include "commands/commands.hpp"

namespace {

struct Builtin {
    std::string_view name;
    int (*func)(const Args &args);
};

const std::vector<Builtin> &builtins() {
    static const std::vector<Builtin> table = {
        {"echo", builtin_echo},
        {"env", builtin_env},
        {"exit", builtin_exit},
    };
    return table;
}

const Builtin *find_builtin(const Args &args) {
    if (args.empty()) {
        return nullptr;
    }
    for (const Builtin &b : builtins()) {
        if (b.name == args.front()) {
            return &b;
        }
    }
    return nullptr;
}

class Pipe {
public:
    Pipe() {
        if (::pipe(fds_) == -1) {
            perror("pipe");
            throw std::runtime_error("pipe");
        }
    }
    ~Pipe() {
        close_read();
        close_write();
    }
    Pipe(const Pipe &) = delete;
    Pipe &operator=(const Pipe &) = delete;
    Pipe(Pipe &&other) noexcept
        : fds_{std::exchange(other.fds_[0], -1), std::exchange(other.fds_[1], -1)} {}
    Pipe &operator=(Pipe &&other) noexcept {
        if (this != &other) {
            close_read();
            close_write();
            fds_[0] = std::exchange(other.fds_[0], -1);
            fds_[1] = std::exchange(other.fds_[1], -1);
        }
        return *this;
    }
    int read_end() const { return fds_[0]; }
    int write_end() const { return fds_[1]; }
    void close_read() {
        if (fds_[0] != -1) {
            ::close(fds_[0]);
            fds_[0] = -1;
        }
    }
    void close_write() {
        if (fds_[1] != -1) {
            ::close(fds_[1]);
            fds_[1] = -1;
        }
    }

private:
    int fds_[2] = {-1, -1};
};

class Child {
public:
    Child() = default;
    explicit Child(pid_t pid) : pid_(pid) {}
    ~Child() { reap(); }
    Child(const Child &) = delete;
    Child &operator=(const Child &) = delete;
    Child(Child &&other) noexcept : pid_(std::exchange(other.pid_, -1)) {}
    Child &operator=(Child &&other) noexcept {
        if (this != &other) {
            reap();
            pid_ = std::exchange(other.pid_, -1);
        }
        return *this;
    }
    pid_t pid() const { return pid_; }
    void reap() {
        if (pid_ > 0) {
            while (::waitpid(pid_, nullptr, 0) == -1 && errno == EINTR) {
            }
            pid_ = -1;
        }
    }
    void terminate() {
        if (pid_ > 0) {
            ::kill(pid_, SIGKILL);
            reap();
        }
    }

private:
    pid_t pid_ = -1;
};

[[noreturn]] void child_execve(const Args &args) {
    std::vector<char *> argv;
    argv.reserve(args.size() + 1);
    for (const std::string &arg : args) {
        argv.push_back(const_cast<char *>(arg.c_str()));
    }
    argv.push_back(nullptr);
    ::execvp(argv.front(), argv.data());
    perror("execvp");
    _exit(EXIT_FAILURE);
}

[[noreturn]] void child_run(const Args &args, size_t index, std::vector<Pipe> &pipes) {
    if (index > 0) {
        ::dup2(pipes[index - 1].read_end(), STDIN_FILENO);
    }
    if (index < pipes.size()) {
        ::dup2(pipes[index].write_end(), STDOUT_FILENO);
    }
    for (Pipe &p : pipes) {
        p.close_read();
        p.close_write();
    }
    if (const Builtin *b = find_builtin(args)) {
        if (b->name == "exit") {
            _exit(EXIT_SUCCESS);
        }
        b->func(args);
        std::cout.flush();
        std::cerr.flush();
        _exit(EXIT_SUCCESS);
    }
    child_execve(args);
}

void exec_command(const Args &args) {
    if (args.empty()) {
        return;
    }
    if (const Builtin *b = find_builtin(args)) {
        b->func(args);
        std::cout.flush();
        return;
    }
    std::cout.flush();
    std::cerr.flush();
    Child child(::fork());
    if (child.pid() == 0) {
        child_execve(args);
    }
    if (child.pid() < 0) {
        perror("fork");
    }
}

void run_pipeline(const std::vector<Args> &commands) {
    const size_t count = commands.size();
    std::vector<Pipe> pipes(count - 1);
    std::vector<Child> children;
    children.reserve(count);

    std::cout.flush();
    std::cerr.flush();

    for (size_t i = 0; i < count; i++) {
        pid_t pid = ::fork();
        if (pid < 0) {
            perror("fork");
            for (Child &c : children) {
                c.terminate();
            }
            return;
        }
        if (pid == 0) {
            child_run(commands[i], i, pipes);
        }
        children.emplace_back(pid);
    }

    for (Pipe &p : pipes) {
        p.close_read();
        p.close_write();
    }
    for (Child &c : children) {
        c.reap();
    }
}

Args to_args(char **tokens) {
    Args args;
    for (int j = 0; tokens[j] != nullptr; j++) {
        args.emplace_back(tokens[j]);
    }
    return args;
}

}  // namespace

extern "C" void exec_pipeline(pipeline_t *pipeline) {
    if (pipeline == nullptr || pipeline->num_commands == 0) {
        return;
    }
    try {
        std::vector<Args> commands;
        commands.reserve(static_cast<size_t>(pipeline->num_commands));
        for (int i = 0; i < pipeline->num_commands; i++) {
            Args args = to_args(pipeline->commands[i]);
            if (!args.empty()) {
                commands.push_back(std::move(args));
            }
        }
        if (commands.empty()) {
            return;
        }
        if (commands.size() == 1) {
            exec_command(commands.front());
        } else {
            run_pipeline(commands);
        }
    } catch (const std::exception &e) {
        std::cerr << "catshell: " << e.what() << '\n';
    }
}

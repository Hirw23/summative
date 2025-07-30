#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_COMMANDS 16

// Function prototypes
void shell_loop();
char* read_input();
char** parse_input(char* input);
int execute_command(char** args);
int execute_builtin(char** args);
void handle_redirection(char** args);
void handle_pipes(char* input);
void execute_single_command(char** args);

// Built-in command functions
int shell_cd(char** args);
int shell_pwd(char** args);
int shell_echo(char** args);
int shell_exit(char** args);
int shell_help(char** args);

// Built-in command names and functions
char* builtin_commands[] = {
    "cd",
    "pwd", 
    "echo",
    "exit",
    "help"
};

int (*builtin_functions[])(char**) = {
    &shell_cd,
    &shell_pwd,
    &shell_echo,
    &shell_exit,
    &shell_help
};

int num_builtins() {
    return sizeof(builtin_commands) / sizeof(char*);
}

int main() {
    printf("=== Mini Shell ===\n");
    printf("Type 'help' for available commands\n");
    
    // Ignore SIGINT (Ctrl+C) for the shell process
    signal(SIGINT, SIG_IGN);
    
    shell_loop();
    
    return 0;
}

void shell_loop() {
    char* input;
    char** args;
    int status = 1;
    
    do {
        printf("minishell> ");
        input = read_input();
        
        // Check for pipes first
        if (strchr(input, '|') != NULL) {
            handle_pipes(input);
        } else {
            args = parse_input(input);
            if (args[0] != NULL) {
                status = execute_command(args);
            }
            free(args);
        }
        
        free(input);
    } while (status);
}

char* read_input() {
    char* input = malloc(MAX_INPUT);
    if (!input) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    
    if (fgets(input, MAX_INPUT, stdin) == NULL) {
        if (feof(stdin)) {
            printf("\nGoodbye!\n");
            exit(0);
        } else {
            perror("read_input");
            exit(1);
        }
    }
    
    // Remove newline character
    input[strcspn(input, "\n")] = '\0';
    
    return input;
}

char** parse_input(char* input) {
    int position = 0;
    char** tokens = malloc(MAX_ARGS * sizeof(char*));
    char* token;
    
    if (!tokens) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    
    token = strtok(input, " \t\r\n\a");
    while (token != NULL && position < MAX_ARGS - 1) {
        tokens[position] = token;
        position++;
        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    
    return tokens;
}

int execute_command(char** args) {
    if (args[0] == NULL) {
        return 1; // Empty command
    }
    
    // Check if it's a built-in command
    int builtin_result = execute_builtin(args);
    if (builtin_result != -1) {
        return builtin_result;
    }
    
    // Check for redirection operators
    int has_redirection = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0) {
            has_redirection = 1;
            break;
        }
    }
    
    if (has_redirection) {
        handle_redirection(args);
    } else {
        execute_single_command(args);
    }
    
    return 1;
}

int execute_builtin(char** args) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_commands[i]) == 0) {
            return (*builtin_functions[i])(args);
        }
    }
    return -1; // Not a built-in command
}

void handle_redirection(char** args) {
    int input_redirect = -1, output_redirect = -1;
    char* input_file = NULL;
    char* output_file = NULL;
    int arg_count = 0;
    
    // Count arguments and find redirection operators
    while (args[arg_count] != NULL) {
        if (strcmp(args[arg_count], "<") == 0) {
            input_redirect = arg_count;
            input_file = args[arg_count + 1];
            args[arg_count] = NULL; // Remove redirection from args
        } else if (strcmp(args[arg_count], ">") == 0) {
            output_redirect = arg_count;
            output_file = args[arg_count + 1];
            args[arg_count] = NULL; // Remove redirection from args
        }
        arg_count++;
    }
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process - handle redirection here
        
        // Handle input redirection
        if (input_redirect != -1 && input_file != NULL) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("Input redirection failed");
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        
        // Handle output redirection
        if (output_redirect != -1 && output_file != NULL) {
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1) {
                perror("Output redirection failed");
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        
        // Execute the command
        if (execvp(args[0], args) == -1) {
            perror("Command execution failed");
        }
        exit(1);
    } else if (pid > 0) {
        // Parent process - wait for child
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("Fork failed");
    }
}

void handle_pipes(char* input) {
    char* commands[MAX_COMMANDS];
    int num_commands = 0;
    
    // Split input by pipes
    char* token = strtok(input, "|");
    while (token != NULL && num_commands < MAX_COMMANDS) {
        commands[num_commands] = token;
        num_commands++;
        token = strtok(NULL, "|");
    }
    
    int pipe_fds[2];
    int input_fd = 0;
    
    for (int i = 0; i < num_commands; i++) {
        char** args = parse_input(commands[i]);
        
        if (i < num_commands - 1) {
            // Create pipe for all but the last command
            if (pipe(pipe_fds) == -1) {
                perror("Pipe creation failed");
                return;
            }
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (input_fd != 0) {
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }
            
            if (i < num_commands - 1) {
                dup2(pipe_fds[1], STDOUT_FILENO);
                close(pipe_fds[1]);
                close(pipe_fds[0]);
            }
            
            if (execute_builtin(args) == -1) {
                execvp(args[0], args);
                perror("Command execution failed");
                exit(1);
            }
            exit(0);
        } else if (pid > 0) {
            // Parent process
            if (input_fd != 0) {
                close(input_fd);
            }
            
            if (i < num_commands - 1) {
                close(pipe_fds[1]);
                input_fd = pipe_fds[0];
            }
            
            wait(NULL);
        } else {
            perror("Fork failed");
        }
        
        free(args);
    }
}

void execute_single_command(char** args) {
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("Command execution failed");
        }
        exit(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("Fork failed");
    }
}

// Built-in command implementations
int shell_cd(char** args) {
    if (args[1] == NULL) {
        // No argument, go to home directory
        if (chdir(getenv("HOME")) != 0) {
            perror("cd");
        }
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
    return 1;
}

int shell_pwd(char** args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
    return 1;
}

int shell_echo(char** args) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s", args[i]);
        if (args[i + 1] != NULL) {
            printf(" ");
        }
    }
    printf("\n");
    return 1;
}

int shell_exit(char** args) {
    printf("Goodbye!\n");
    return 0;
}

int shell_help(char** args) {
    printf("=== Mini Shell Help ===\n");
    printf("Built-in commands:\n");
    printf("  cd [directory]    - Change directory\n");
    printf("  pwd              - Print working directory\n");
    printf("  echo [text]      - Print text\n");
    printf("  exit             - Exit the shell\n");
    printf("  help             - Show this help\n");
    printf("\nSupported features:\n");
    printf("  - External commands (ls, cat, touch, etc.)\n");
    printf("  - Input redirection: command < file\n");
    printf("  - Output redirection: command > file\n");
    printf("  - Piping: command1 | command2\n");
    printf("\nExamples:\n");
    printf("  ls -la\n");
    printf("  cat file.txt\n");
    printf("  echo 'Hello World' > output.txt\n");
    printf("  cat < input.txt\n");
    printf("  ls | grep txt\n");
    printf("  ps aux | grep bash | wc -l\n");
    return 1;
}

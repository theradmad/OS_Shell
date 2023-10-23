#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128
#define MAX_EXECUTION_TIME 10 // Maximum execution time in seconds

char delimiters[] = " \t\r\n";

void execute_command(char *arguments[], bool background) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        // Check for input redirection
        int input_index = -1;
        for (int i = 0; arguments[i] != NULL; i++) {
            if (strcmp(arguments[i], "<") == 0) {
                input_index = i;
                break;
            }
        }

        if (input_index != -1) {
            // Input redirection detected
            char *input_file = arguments[input_index + 1];
            int fd = open(input_file, O_RDONLY);
            if (fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            dup2(fd, STDIN_FILENO);
            close(fd);

            // Remove the input redirection symbols and file
            arguments[input_index] = NULL;
            arguments[input_index + 1] = NULL;
        }

        // Check for output redirection
        int output_index = -1;
        for (int i = 0; arguments[i] != NULL; i++) {
            if (strcmp(arguments[i], ">") == 0) {
                output_index = i;
                break;
            }
        }

        if (output_index != -1) {
            // Output redirection detected
            char *output_file = arguments[output_index + 1];
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            dup2(fd, STDOUT_FILENO);
            close(fd);

            // Remove the output redirection symbols and file
            arguments[output_index] = NULL;
            arguments[output_index + 1] = NULL;
        }

        if (!background) {
            if (execvp(arguments[0], arguments) == -1) {
                perror("execvp");


void signal_handler(int signo) {
    if (signo == SIGINT) {
        printf("\n"); // Print a newline after receiving SIGINT (Ctrl+C)
    }
}

// Function to kill a process if it exceeds the time limit
void kill_long_running_process(pid_t pid) {
    kill(pid, SIGTERM);
    printf("Process terminated: Execution time exceeded the limit.\n");
}


int main() {
    char command_line[MAX_COMMAND_LINE_LEN];
    char *arguments[MAX_COMMAND_LINE_ARGS];
    char *token;

    signal(SIGINT, signal_handler);

    while (true) {
        // Print the shell prompt with the current working directory
        char cwd[MAX_COMMAND_LINE_LEN];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        } else {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }

        // Read input from stdin
        if (fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // Remove trailing newline character
        command_line[strcspn(command_line, "\n")] = '\0';

        // Tokenize the command line input
        int i = 0;
        token = strtok(command_line, delimiters);
        while (token != NULL) {
            arguments[i++] = token;
            token = strtok(NULL, delimiters);
        }
        arguments[i] = NULL;

        // Check for background process
        bool background = false;
        if (i > 0 && strcmp(arguments[i - 1], "&") == 0) {
            background = true;
            arguments[i - 1] = NULL; // Remove the '&' from arguments
        }

        // Expand environment variables
        for (int j = 0; arguments[j] != NULL; j++) {
            if (arguments[j][0] == '$') {
                expand_env(arguments[j]);
            }
        }

        // Implement Built-In Commands
        if (strcmp(arguments[0], "cd") == 0) {
            if (arguments[1] != NULL) {
                if (chdir(arguments[1]) != 0) {
                    perror("cd");
                }
            } else {
                fprintf(stderr, "cd: missing argument\n");
            }
        } else if (strcmp(arguments[0], "pwd") == 0) {
            printf("%s\n", cwd);
        } else if (strcmp(arguments[0], "echo") == 0) {
            for (int j = 1; arguments[j] != NULL; j++) {
                printf("%s ", arguments[j]);
            }
            printf("\n");
        } else if (strcmp(arguments[0], "exit") == 0) {
            return 0;
        } else if (strcmp(arguments[0], "env") == 0) {
            extern char **environ;
            for (int j = 0; environ[j] != NULL; j++) {
                printf("%s\n", environ[j]);
            }
        } else if (strcmp(arguments[0], "setenv") == 0) {
            if (arguments[1] != NULL && arguments[2] != NULL) {
                if (setenv(arguments[1], arguments[2], 1) != 0) {
                    perror("setenv");
                }
            } else {
                fprintf(stderr, "setenv: missing argument(s)\n");
            }
        } else {
            // Execute the command
            execute_command(arguments, background);
        }
    }

    return 0;
}

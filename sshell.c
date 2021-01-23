#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CMDLINE_MAX 512
#define ARGS_MAX 16

int outputRedirection(char *args[ARGS_MAX], int argIndex){
    int arrowIndex= -1;
    for(int i=0; i<argIndex; i++){
        if (!strcmp(args[i], ">")){
            arrowIndex = i;
            args[arrowIndex] = NULL;
        }
    }
    
    if (arrowIndex > -1){
        int fd = open(args[arrowIndex+1],O_WRONLY | O_CREAT | O_TRUNC , 0644);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    return 1;
}

int main(void)
{
    char cmd[CMDLINE_MAX];
    while (1) {
        char *nl;
        int retval = 0;
        
        // Print prompt
        printf("sshell@ucd$ ");
        fflush(stdout);
        
        // Get command line
        fgets(cmd, CMDLINE_MAX, stdin);
        
        // Print command line if stdin is not provided by terminal
        if (!isatty(STDIN_FILENO)) {
            printf("%s", cmd);
            fflush(stdout);
        }
        
        // Remove trailing newline from command line
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';
        
        // Builtin exit command
        if (!strcmp(cmd, "exit")) {
            fprintf(stderr, "Bye...\n");
            fprintf(stderr, "+ completed '%s' [%d]\n", cmd, retval);
            break;
        }
        
        char *args[ARGS_MAX],
        *delim = " \n",
        *token,
        *command;
        int argIndex = 0,
        arrow_index = 0;
        command = strdup(cmd);
        
        for (int i = 0; i < ARGS_MAX; i++)
            args[i] = NULL;
        
        // parsing by space
        for (token = strtok (command, delim); token && argIndex + 1 < ARGS_MAX; token = strtok (NULL, delim)) {
            if (!strcmp(token, ">")){
                arrow_index = argIndex;
            }
            args[argIndex++] = token;
        }
        
        // Builtin cd command
        int result = 0;
        if (!strcmp(command, "cd")){
             result = chdir(args[1]);
            if (result != 0){
                fprintf(stderr, "Error cannot cd into directory\n");
            }
            else {
                fprintf(stderr, "+ completed '%s' [%d]\n", cmd, retval);
            }
            continue;
        }
        
        // Builtin pwd command
        if (!strcmp(command, "pwd")) {
           char *getcwd(char *buf, size_t size);
           char cwd[CMDLINE_MAX];
           if (getcwd(cwd, sizeof(cwd)) != NULL) {
               printf("%s\n", cwd);
               fprintf(stderr, "+ completed '%s' [%d]\n", cmd, retval);
           } else {
               perror("getcwd() error");
           }
            continue;
        }
        
        // Error management
        if  (argIndex > 14) {
            fprintf(stderr, "Error: too many arguments\n");
            continue;
        }
        
        if (!strcmp(command, ">")){
            fprintf(stderr, "Error: missing command\n");
            continue;
        }
        
        if(args[arrow_index + 1] == NULL && arrow_index != 0){
            fprintf(stderr, "Error: no output file\n");
            continue;
        }

        // fork, exec, wait
        pid_t pid = fork();
        if (pid == 0) {
            outputRedirection(args, argIndex);
            execvp(command, args);
            fprintf(stderr, "Error: command not found");
            exit(1);
        } else if (pid > 0 ) {
            // parent execution
            int status;
            waitpid(pid, &status, 0); // suspends execution of current process until child has exited
            fprintf(stderr, "+ completed '%s' [%d]\n", cmd, WEXITSTATUS(status));
        } else {
            perror("fork");
            exit(1);
        }
    }
    return EXIT_SUCCESS;
}

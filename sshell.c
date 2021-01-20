#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CMDLINE_MAX 512
#define ARGS_MAX 16

struct parsedCommand {
    char *command;
    char *parsed;
    char **arguments;
    int argCount;
};


// struct is not being used to handle parsing. parsing starts on line 104
struct parsedCommand input_parser(char *command)
{
    //        struct parsedCommand parseobject = {NULL};
    //        int i = 0;
    //        char  * token = strtok(input, " ");
    //        while (token != NULL) {
    //            parseobject.argumentsArray[i] = token;
    //            token = strtok(NULL, " ");
    //            i++;
    //        }
    //        parseobject.argumentsArray[i]= NULL;
    //        return parseobject;
    
    char *cmdArgs[ARGS_MAX];
    struct parsedCommand input;
    input.command = strdup(command);
    input.parsed = strtok(input.command, " ");
    input.argCount = 0;
    for (char *arg = strtok(NULL, " "); arg; arg = strtok(NULL, " ")){
        cmdArgs[input.argCount++] = arg;
    }
    input.arguments = malloc(input.argCount * sizeof *input.arguments);
    memcpy(input.arguments, cmdArgs, input.argCount * sizeof *input.arguments);
    return input;
    
}




int main(void)
{
    char cmd[CMDLINE_MAX];
    
    while (1) {
        char *nl;
        //int retval;
        
        /* Print prompt */
        printf("sshell@ucd$ ");
        fflush(stdout);
        
        /* Get command line */
        fgets(cmd, CMDLINE_MAX, stdin);
        
        
        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO)) {
            printf("%s", cmd);
            fflush(stdout);
        }
        
        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';
        
        /* Builtin exit command */
        if (!strcmp(cmd, "exit")) {
            fprintf(stderr, "Byeeee...\n");
            break;
        }
        
        
//        *********** ignore **************
//        struct parsedCommand input = input_parser(cmd);
//        printf("Command (%d args): %s", input.argCount, input.parsed);
//        for (int i = 0; i < input.argCount; ++i)
//            printf(" %s", input.arguments[i]);
//        printf("\n");
//
//
//        char *argument = NULL;
//        for (int i = 0; i < input.argCount; ++i)
//            argument = input.arguments[i];
//        char *args[] = {input.parsed, argument, NULL};
//
//        if (!strcmp(input.parsed, "cd")){
//            chdir(input.arguments[0]);
//        }
        
        char *args[ARGS_MAX],
        *delim = " \n",
        *token;
        int argIndex = 0;
        char *command;
        
        command = strdup(cmd);
        
        
//        if the command has an > in it, then you should add white space around the >
//        int i =0;
//        while(cmd[i] != '\0'){
//            if (!strcmp(&cmd[i], ">")){
//                strcpy(&cmd[i], " > ");
//            }
//            i++;
//        }
        
        for (int i = 0; i < ARGS_MAX; i++)  /* set all pointers NULL */
            args[i] = NULL;
        
        for (token = strtok (command, delim); token && argIndex + 1 < ARGS_MAX; token = strtok (NULL, delim)) {
            args[argIndex++] = token;
            
        }
        
//      ********* ignore *********
//        if(strstr(cmd, ">")  !=NULL ){
//            redirection(cmd);
//        }
        // if string contains > then call redirection function
        // inside redirection, parse to find argument that gives file name.
        // then call dup2 using that file name
        
        if (!strcmp(command, "cd")){
            chdir(args[1]);
        }
        
        
        // fork, wait, exec
        pid_t pid = fork();
        if (pid == 0) {
            int arrowIndex= -1;
            
            
            for(int i=0; i<argIndex; i++){
                if (!strcmp(args[i], ">")){
                    arrowIndex = i;
                }
            }
            
            if (args[arrowIndex + 1] == NULL){
                printf("Error: no output file\n");
                return 0;
            }
            if (arrowIndex > -1){
                int fd = open(args[arrowIndex+1],O_WRONLY | O_CREAT | O_TRUNC , 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            
            args[arrowIndex] = NULL;
            execvp(command, args);
            perror("execvp");
            exit(1);
        } else if (pid > 0 ) {
            // parent execution
            int status;
            waitpid(pid, &status, 0); // suspends execution of current process until child has exited/signaled
            fprintf(stderr, "+ completed '%s' [%d]\n\n", cmd , WEXITSTATUS(status));
        } else {
            perror("fork");
            exit(1);
        }
        
        
        // prints working directory
        if (!strcmp(cmd, "pwd")) {
            char *getcwd(char *buf, size_t size);
            char cwd[CMDLINE_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("Current working directory: %s\n", cwd);
            } else {
                perror("getcwd() error");
            }
        }
    }
    return EXIT_SUCCESS;
}

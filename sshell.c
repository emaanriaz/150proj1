#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CMDLINE_MAX 512

int echo(){
    char *cmd = "/bin/echo";
    char *args[] = {cmd, "hi", NULL};
    pid_t pid = fork();

    if (pid == 0) {
       // child execution
       execv(cmd, args);
       perror("execv");
       exit(1);
    } else if (pid > 0 ) {
       // parent execution
       int status;
       waitpid(pid, &status, 0); // suspends execution of current process until child has exited/signaled
       printf("Child returned %d\n", WEXITSTATUS(status));
    } else {
       perror("fork");
       exit(1);
    }
    
    return 0;
}


char *cmd = "/bin/ls",  "-l", "

int main(void)
{
    char cmd[CMDLINE_MAX];

    while (1) {
        char *nl;
//            int retval;

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

        /* Builtin command */
        if (!strcmp(cmd, "exit")) {
                fprintf(stderr, "Byeeee...\n");
                break;
        }

        /* Regular command */
//                retval = system(cmd);
         
        if (!strcmp(cmd, "echo")) {
            echo();
        }
        
        if (!strcmp(cmd, "ls")) {
            ls();
        }
        
   
        
//            fprintf(stdout, "Return status value for '%s': %d\n",
//                    cmd, retval);
    }

    return EXIT_SUCCESS;
}

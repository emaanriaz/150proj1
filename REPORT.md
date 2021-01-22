# REPORT.md

We implemented our shell by following a series of phases:

In phase 1, we gave the shell the ability to run simple commands with no
arguments. To do this, we implemented the fork+exec+wait method. We used execvp 
so that programs could be searched according to the $PATH environment
variable and users would not have to type the complete path.

In phase 2 we gave the shell the ability to handle command line arguments by
adding a parser. The parser takes in a command from the shell and separates
the line by whitespace. The resulting separated strings are put into an array.
We used an array to store the arguments because it allowed us to easily access
each argument. 

In phase 3 we added functionality for the built in commands cd, pwd, and exit.
For each of these commands, we compared the user’s input to one of the built 
in commands and executed the necessary function. Each of these checks occur 
inside the while loop in the main function so that once the check is complete,
we can use a continue statement to skip the fork and force the next iteration 
of the loop. 

In phase 4 we gave the shell the ability to redirect its output to a specified
file. We make a call to the outputRedirection function inside the child 
process of the fork so that. When we call the output redirection function from 
main, we want to make sure that output redirection only takes place when the 
command contains >.  So, inside the output redirection function we first 
create a variable called arrowIndex and set it to -1. Then, we loop through
the arguments to compare each argument to >. If > is present, we set the 
loop’s current index equal to arrowIndex so that we can keep track of the 
position of the > in the array.  Now that we have the position of the > in the 
array, we know for sure that the arrowIndex has to be greater than -1. Thus, 
we can safely open the file and call dup2 to redirect the output. We know that 
the filename from the command line input will be directly after the >, so we 
can represent that position by adding 1 to arrowIndex. 

To test our shell, we initially used a trial and error method to ensure that 
our output was the same as the output in the reference shell. Then we used 
the tester shell to perform automatic testing. 

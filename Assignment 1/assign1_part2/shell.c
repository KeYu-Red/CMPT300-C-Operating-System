/* SMP1: Simple Shell */

/* LIBRARY SECTION */
#include <ctype.h>              /* Character types                       */
#include <stdio.h>              /* Standard buffered input/output        */
#include <stdlib.h>             /* Standard library functions            */
#include <string.h>             /* String operations                     */
#include <sys/types.h>          /* Data types                            */
#include <sys/wait.h>           /* Declarations for waiting              */
#include <unistd.h>             /* Standard symbolic constants and types */

#include "smp1_tests.h"         /* Built-in test system                  */

/* DEFINE SECTION */
#define SHELL_BUFFER_SIZE 256   /* Size of the Shell input buffer        */
#define SHELL_MAX_ARGS 8        /* Maximum number of arguments parsed    */



/* VARIABLE SECTION */
enum { STATE_SPACE, STATE_NON_SPACE };	/* Parser states */




int imthechild(const char *path_to_exec, char *const args[])
{

	
	return execv(path_to_exec, args) ? -1 : 0;
}

void imtheparent(pid_t child_pid, int run_in_background)
{
	int child_return_val, child_error_code;

	/* fork returned a positive pid so we are the parent */
	fprintf(stderr,
	        "  Parent says 'child process has been forked with pid=%d'\n",
	        child_pid);
	if (run_in_background) {
		fprintf(stderr,
		        "  Parent says 'run_in_background=1 ... so we're not waiting for the child'\n");
		return;
	}
	waitpid(-1,&child_return_val,0);
	//wait(&child_return_val);
	/* Use the WEXITSTATUS to extract the status code from the return value */
	child_error_code = WEXITSTATUS(child_return_val);
	fprintf(stderr,
	        "  Parent says 'wait() returned so the child with pid=%d is finished.'\n",
	        child_pid);
	if (child_error_code != 0) {
		/* Error: Child process failed. Most likely a failed exec */
		fprintf(stderr,
		        "  Parent says 'Child process %d failed with code %d'\n",
		        child_pid, child_error_code);
	}
}

/* MAIN PROCEDURE SECTION */
int main(int argc, char **argv)
{
	pid_t shell_pid, pid_from_fork;
	int n_read, i, exec_argc, parser_state, run_in_background;
	/* buffer: The Shell's input buffer. */
	char buffer[SHELL_BUFFER_SIZE];
	/* exec_argv: Arguments passed to exec call including NULL terminator. */
	char *exec_argv[SHELL_MAX_ARGS + 1];
	//store the command line
	char exec_store[100][SHELL_MAX_ARGS+ 1][SHELL_BUFFER_SIZE];
	int  exec_argc_num[100]= {0};

	//Global Varable of Subshells
	int NumberOfSubshells = 0;
	/* Entrypoint for the testrunner program */
	if (argc > 1 && !strcmp(argv[1], "-test")) {
		return run_smp1_tests(argc - 1, argv + 1);
	}
	int counter = 1;
	/* Allow the Shell prompt to display the pid of this process */
	shell_pid = getpid();

	while (1) {
	/* The Shell runs in an infinite loop, processing input. */
		//printf("***************************\n");
		shell_pid = getpid();
		fprintf(stdout, "Shell(pid=%d)%d> ", shell_pid, counter); //"Shell(pid=%1)%2> "  %1=process pid %2=counter
		fflush(stdout);
		
		/* Read a line of input. */
		if (fgets(buffer, SHELL_BUFFER_SIZE, stdin) == NULL)
			return EXIT_SUCCESS;
		n_read = strlen(buffer);
		run_in_background = n_read > 2 && buffer[n_read - 2] == '&';
		buffer[n_read - run_in_background - 1] = '\n';

		/* Parse the arguments: the first argument is the file or command *
		 * we want to run.                                                */

		parser_state = STATE_SPACE;
		for (exec_argc = 0, i = 0;
		     (buffer[i] != '\n') && (exec_argc < SHELL_MAX_ARGS); i++) {

			if (!isspace(buffer[i])) {
				if (parser_state == STATE_SPACE)
					exec_argv[exec_argc++] = &buffer[i];
				parser_state = STATE_NON_SPACE;
			} else {
				buffer[i] = '\0';
				parser_state = STATE_SPACE;
			}
		}

		//for(int ii=1; ii<counter; ii++){
		//	printf("exec_store[%d][0]=%s \n",ii,exec_store[ii][0]);
		//}
		/* run_in_background is 1 if the input line's last character *
		 * is an ampersand (indicating background execution).        */


		buffer[i] = '\0';	/* Terminate input, overwriting the '&' if it exists */

		/* If no command was given (empty line) the Shell just prints the prompt again */
		if (!exec_argc)
			continue;
		/* Terminate the list of exec parameters with NULL */
		exec_argv[exec_argc] = NULL;

		/* If Shell runs 'exit' it exits the program. */

		//printf("exec_store[%d][0]=%s exec_store[%d][0]=%s\n",counter-1,exec_store[counter-1][0],counter-1,exec_store[counter-1][1]);

		if(exec_argv[0][0]=='!'){
			int exec_num = exec_argv[0][1]-'0';
			//printf("**********  1   ******\n");
			if(exec_num >= counter){
				fprintf(stderr,"Not valid\n");
				//printf("**********  2   ******\n");
				continue;
			}else{
				exec_argc = exec_argc_num[exec_num];
				//printf("**********  3   ******\n");
				for(int i=0; i < exec_argc; i++){
					//strcpy(exec_argv[i],exec_store[exec_num][i]);
					exec_argv[i] = exec_store[exec_num][i];
				}
				//printf("**********  4   ******\n");
				exec_argv[exec_argc] = NULL;
			}
		}

		if (!strcmp(exec_argv[0], "exit")) {
			printf("Exiting process %d\n", shell_pid);
			NumberOfSubshells--;
			return EXIT_SUCCESS;	/* End Shell program */
		} else if (!strcmp(exec_argv[0], "cd") && exec_argc > 1) {
		/* Running 'cd' changes the Shell's working directory. */
			//if(fork()==0){
			/* Alternative: try chdir inside a forked child: if(fork() == 0) { */
				if (chdir(exec_argv[1]))
					/* Error: change directory failed */
					fprintf(stderr, "cd: failed to chdir %s\n", exec_argv[1]);	
				/* End alternative: exit(EXIT_SUCCESS);} */
				for(int i=0; i < exec_argc;i++){
				strcpy(exec_store[counter][i],exec_argv[i]);
			}
				exec_argc_num[counter] = exec_argc;
				//printf("exec_store[%d][0]=%s exec_store[%d][0]=%s\n",counter,exec_store[counter][0],counter,exec_store[counter][1]);
				counter++;
				exit(EXIT_SUCCESS);
			//}
		}else if(!strcmp(exec_argv[0],"sub")){
			if(NumberOfSubshells < 2){
				NumberOfSubshells++;
				pid_from_fork = fork();
				if(pid_from_fork > 0){	
					//printf("NumberOfSubshells = %d\n",NumberOfSubshells);
					imtheparent(pid_from_fork, run_in_background);
				}
			}else{
				fprintf(stderr,"Too deep!\n");
				continue;
			}		
		}
		else {
		/* Execute Commands */
			/* Try replacing 'fork()' with '0'.  What happens? */
			pid_from_fork = fork();
			//pid_from_fork = 0;
			if (pid_from_fork < 0) {
				/* Error: fork() failed.  Unlikely, but possible (e.g. OS *
				 * kernel runs out of memory or process descriptors).     */
				fprintf(stderr, "fork failed\n");
				continue;
			}
			if (pid_from_fork == 0) {
				char s[3] = "ls";
				char command[8] = "/bin/ls";
				
				if(!strcmp(exec_argv[0],s)){
					//strcpy(exec_store[counter][0],exec_argv[0]);
					//printf("exec_store[%d][0]=%s \n",counter,exec_store[counter][0]);
					//counter++;
					return imthechild(command,&exec_argv[0]);
				}else {
					//strcpy(exec_store[counter][0],exec_argv[0]);
					//printf("exec_store[%d][0]=%s \n",counter,exec_store[counter][0]);
					return imthechild(exec_argv[0], &exec_argv[0]);
				/* Exit from main. */
				}
			} else {
				 for(int i=0; i < exec_argc;i++){
				 strcpy(exec_store[counter][i],exec_argv[i]);
			 }
				 exec_argc_num[counter] = exec_argc;
				
				 imtheparent(pid_from_fork, run_in_background);
				/* Parent will continue around the loop. */
			}

			counter++;
		} /* end if */
		//for(int i=0;i<exec_argc;i++){
			//printf("%s",exec_store[counter-1][i]);
		//}
		//printf("\n");
		

	} 
	
	/* end while loop */

	return EXIT_SUCCESS;
} /* end main() */

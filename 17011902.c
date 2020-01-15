
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

/* The array below will hold the arguments: args[0] is the command. */
static char* args[512];
#define HISTORY_COUNT 30
static int command()
{	pid_t pid;
	pid = fork(); //forks the process, and saves the return value
 	// Once fork() returns, we actually have two processes running concurrently. 
 
	if (pid == 0) { //In the child process, we want to run the command given by the user. 
		
		if (execvp( args[0], args) == -1)//child uses exec() to replace itself with a new program.
/*Expects a program name and an array  of string arguments (the first one has to be the program name). The ‘p’ means that instead of providing the full file path of the program to run, we’re going to give its name, and let the operating system search for the program in the path.*/
			_exit(EXIT_FAILURE); // If child fails 
		//Then, we exit so that the shell can keep running.
	}else if (pid < 0) {
    	// Error forking
/*checks whether fork() had an error. If so, we print it and keep going – there’s no handling that error beyond telling the user and letting them decide if they need to quit.*/
   	 perror("fork");
 	 }
	return 1;
}
 
/* Final cleanup, 'wait' for processes to terminate.
 *  n : Number of times 'command' was invoked.
 */
static void cleanup(int n)
{
	int i;
	for (i = 0; i < n; ++i) 
		wait(NULL); 
}
 
static int run(char* cmd);
static char line[1024];
static int n = 0; /* number of calls to 'command' */
 
int main()
{	printf("\033[0;31m"); 
	printf("SHELL: Type 'exit' or send EOF to exit.\n");
	printf("\033[0m"); 
	while (1) {
		/* Print the command prompt */
		printf("\033[1;32m"); 
		printf("nina@nina\033[0m:\033[1;34m~\033[0m$ "); 
		
		/* Read a command line */
		if (!fgets(line, 1024, stdin)) 
			return 0;
		int input = 0;
 		char* cmd = line;
		input = run(cmd);//terminate command
		cleanup(n);//'wait' for processes to terminate
		n = 0;
	}
	return 0;
}
 
static void split(char* cmd);

int history(char *hist[], int current)//print the history
{
        int i = current;
        int hist_num = 1;

        do {
                if (hist[i]) {
                        printf("%d  %s\n", hist_num, hist[i]);
                        hist_num++;
                }

                i = (i + 1) % HISTORY_COUNT;

        } while (i != current);

        return 0;
}
char *hist[HISTORY_COUNT];
int i, current = 0;
static int run(char* cmd)
{	 char tmpStr[256], c;
    char bin[7];
	split(cmd);
	if (args[0] != NULL) {
		n += 1;
                hist[current] = strdup(cmd);
                current = (current + 1) % HISTORY_COUNT;
		if (strcmp(args[0], "exit") == 0) 
			exit(0);
		if (strcmp(args[0],"history") == 0) 
			history(hist, current);
/* The shell process itself needs to execute chdir(), so that its own current directory is updated. First checks that its second argument exists, and prints an error message if it doesn’t. Then, it calls chdir(), checks for errors, and returns. */
		if (strcmp(args[0], "cd") == 0) {
			if (args[1] == NULL) {
    			fprintf(stderr, "shell: expected argument to \"cd\"\n");
  			} else {
   			 if (chdir(args[1]) != 0) {
    			  perror("shell");
   				 }
 			     }
			}
		 else if (!strcmp(args[0], "clear")) {
			system("clear");
		    }

			 else if (!strcmp(args[0], "echo")) {
			if (strlen(args[1])) {
			  fprintf(stdout, "%s\n", args[1]);
			}
		    } else if (!strcmp(args[0], "mkdir")) {
			strcpy(tmpStr, args[1]);
				if (mkdir(tmpStr, 0777)==-1) {
					printf("Unable to create directory\n"); 
					 }
				    
		    }else if (!strcmp(args[0], "rmdir")) {
			strcpy(tmpStr, args[1]);
				if (remove(tmpStr)==-1) {
					printf("Directory doesn't exists\n"); 
					 }
				   
		    } else if (!strcmp(args[0], "touch")) {
			FILE *fp;
			fp=fopen(args[1],"w");
			fclose(fp);

		    }  else if (!strcmp(args[0], "cat")) {
			FILE *fptr;
			fptr=fopen(args[1],"r");
			c = fgetc(fptr);
			while (c != EOF) {
			    printf ("%c", c);
			    c = fgetc(fptr);
			}
			fclose(fptr);

		    } 
			else
		return command();
	
	}
	return 0;
}
 
static char* skipwhite(char* s)
{
	while (isspace(*s)) ++s;//skip while whitespace not exists,if whitespace exit
	return s;
}
 
static void split(char* cmd)
{
	cmd = skipwhite(cmd);
	char* next = strchr(cmd, ' ');//whitespace adress or null if not exists
	int i = 0;
	while(next != NULL) {
		next[0] = '\0';
		args[i] = cmd;
		++i;
		cmd = skipwhite(next + 1);//next command after  whitespaces 
		next = strchr(cmd, ' ');
	}
 
	if (cmd[0] != '\0') { //if one command get the command directly
		args[i] = cmd;
		next = strchr(cmd, '\n');
		next[0] = '\0';
		++i; 
	}
	args[i] = NULL;
}

// CS 344 - Project 3
// Eric Stevens

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define INPUT_CHARS 2048
#define INPUT_ARGS 512

// Holds information about the command
// most recently received form the line
struct Options {
	int argCount;			// how many arguments there are
	int redirectIn;			// is there a directed input
	int redirectOut;		// is the outupt redirected
	char *readFrom;			// filename of input for redirect
	char *writeTo;			// filename for output redirect
	int putBG;			// put into background flag
};



// gets a line from stdin
char *getCommand();	
// parses the received command into an array of strings		
char **parseCommand(char *input, struct Options *opts);
// responsible for executions of non built in functions
void systemExecution(char **argList, struct Options *opts);
// mannages foreground waiting and signal and error messages
void foreground(pid_t id, char *exitMessage);
// terminates background processes that were not terminated properly
void killBackground();



main(){


	int close = 0;		// if set to one then close smallshell
	int procCount=0;	// number of processes ran
	int procList[20];	// list of process ids
	
	char exitMessage[100] = "nothing";	// keeps track of exit value
		
	struct sigaction normally;		// normal action to take on a signal
	normally.sa_handler = SIG_DFL;		// the default

	struct sigaction ignore;		// when we want to ignore the signal
	ignore.sa_handler = SIG_IGN;		// we ignore it

	sigaction(SIGINT, &ignore, NULL);	// ignore Cntrl+C for the main functions



	// This loop is the shell loop. It will run until the variable 'close' is set.
	do{
		
		struct Options *opts;		// holds information about given command
		char *input = NULL;		// input the line being read from the command line
		char **argList;			// argList will be an array of strings, the parsed input


		// Initalize the command options
		opts = malloc(sizeof(struct Options));	// allocate memory for the structure
		opts->argCount = 0;			// 0 arguments
		opts->putBG = 0;			// dont put in background
		opts->redirectIn = 0;			// dont redirect input
		opts->redirectOut = 0;			// dont redirect output
	
		// kill left over background processes
		killBackground();			


		// BEGIN COMMAND LINE CONTROL
		printf(": "); 				// prompt user
		input = getCommand();			// get user input form command line
		argList = parseCommand(input, opts);	// parse input into array of commands
		
		
		// if enter is pressed without anything entered do nothing
		if(opts->argCount == 0 || argList[0] == NULL){
			close = 0;
		}

		// if the line starts with a '#' then ignore it as a comment
		else if(strncmp(argList[0], "#", 1) == 0){
			close = 0;
		}

		// if the first argumeent received was 'exit':
		else if(strcmp(argList[0], "exit") == 0){
			// if there is more than 
			if(opts->argCount > 1){
				printf("too many arguments\n");
				fflush(stdout);
			}
			// otherwise		
			else{
				close = 1;	// set the variable to close the shell
				
				int i;					// iterator variable
				for(i=0; i<procCount; i++){		// loop through all open processes
					kill(procList[i], SIGTERM);	// kill those processes
				}
			}
		}

		// if the first argument received was 'cd': 
		else if(strcmp(argList[0], "cd") == 0){ 
			// if there are more than 2 arguments
			if(opts->argCount > 2){
				fprintf(stderr, "too many arguments\n");
			}
			// if there is a file name argument but it can not be changed to
			else if(chdir(argList[1]) != 0 && argList[1] != NULL){
				fprintf(stderr, "could not go to %s\n", argList[1]);
			}
			// if it can be changed to, change to it
			else if(opts->argCount == 2){
				chdir(argList[1]);
			}
			// if 'cd' is the only argument, go to home directory
			else{chdir(getenv("HOME"));}
		}

		// if the first arument received was 'status':
		else if(strcmp(argList[0], "status") == 0){
			// print out the message currently stored in the exit message string
			fprintf(stdout, "%s\n", exitMessage);
			fflush(stdout);

		}

		// if none of the above are true, the request was for a non built in command		
		else{
			pid_t id;	// initalize a process id
			id = fork();	// fork off a child
		
			// if this process is a child
			if(id == 0){
				// if the background flag is set take the default actions on sigint
				if(opts->putBG == 0){
					sigaction(SIGINT, &normally, NULL);
				}
				
				// execute the command
				systemExecution(argList, opts);
			}
			
			// if the for failed	
			else if(id == -1){
				perror("fork fail");
				exit(1);
			}
			
			// otherwise the process is a parent	
			else{	
				// if the the child will ba a background process
				if(opts->putBG == 1){
					procList[procCount] = id;		// add it to the process list
					procCount = procCount+1;		// increment the process counter
					printf("background pid is %d\n", id);	// display the background process id
					fflush(stdout);
				}
				
				// otherwise continue to manage foreground processes
				else{foreground(id, exitMessage);}

			}
		

		}			
				
		// dealocation of command space for next loop
		free(argList);
		free(opts);
		free(input);

	}while(!close);



return 0;

}





//////////////////////////////////////////////////////////////////////////
// Function to pull user input off of command line
char *getCommand(){
	
	char *input = NULL;			// string declaration
	size_t buffer = INPUT_CHARS;		// buffer
	getline(&input, &buffer, stdin);	// put line into buffer

	return input;				// return the line as a string

}
//////////////////////////////////////////////////////////////////////////
// Function to split up the given input line by spaces
char **parseCommand(char *input, struct Options *opts){

	int buffer, ioOpt;	// buffer size, number specifies in/out redirection
	char *currentArg;	// argument currently being looked at
	char **argList;		// array of arguments to be returned

	buffer = INPUT_CHARS;	// possible size of input
	ioOpt = 1;		// io redirection
	
	argList = malloc(buffer * sizeof(char *)); // allocate space for argument list
						
	currentArg = strtok(input, " \n\t");	// parse first argument off of input line

	// initalize options 
	opts->putBG = 0;
	opts->argCount = 0;
	opts->redirectIn = 0;
	opts->redirectOut = 0;
	
	// loop to pull arguments off of input line
	while(ioOpt != 0 && currentArg != NULL){
							
		if(strcmp(currentArg, "&") == 0){	// if backgroun process requested
			opts->putBG = 1;		// set the background flag
			ioOpt = 0;			// set background io option
		}

		else if(strcmp(currentArg, "<") == 0){	// if input redirect requested
			opts->redirectIn = 1;		// set input redirect flag
			ioOpt = 2;			// set io option
		}

		else if(strcmp(currentArg, ">") == 0){	// if output redirect requested
			opts->redirectOut = 1;		// set output redirect flag
			ioOpt = 3;			// set io option	
		}
		
		else{								// otherwise
			if(ioOpt == 2){opts->readFrom = currentArg;}		// last arg was < set stdin to this arg	
			else if(ioOpt == 3){opts->writeTo = currentArg;}	// last arg was > set std out to this arg
			else{							// otherwise
				argList[opts->argCount] = currentArg;		// add this to the arg list at argCount
				opts->argCount = opts->argCount + 1;		// incremnt argCount
				argList[opts->argCount] = NULL;			// set the next arg to null
			}
		}
	
		currentArg = strtok(NULL, " \n\t");	// parse off next argument into currentArg
	}

	return argList; // return the list of parsed arument minus the redirect and background commands
}
//////////////////////////////////////////////////////////////////////////
// This function is used to execute commands that are not built into the shell
void systemExecution(char **argList, struct Options *opts){

	int inDescript; 	// file descriptor used for input redirection
	int outDescript;	// file descriptor used for output redirection
	
	char filename[500] = "";	// filename buffer

	if(opts->redirectIn == 1){					// if input redirection flag set
		inDescript = open(opts->readFrom, O_RDONLY);		// set the file descriptor
		if(inDescript == -1 || dup2(inDescript,0) == -1){	// handle failure
			fprintf(stderr, "failure to open %s for reaing\n",opts->readFrom);
			exit(1);
		}
	}
					
	else if(opts->putBG == 1){					// if the background flag set
		
		inDescript = open("/dev/null", O_RDONLY);		// set the input descriptor to
		outDescript = open("/dev/null", O_WRONLY);		// set the output descriptor to
		dup2(inDescript, 0);					// redirect stdin		
		dup2(outDescript, 1);					// redirect stdout
		

		if(inDescript == -1 || dup2(inDescript, 0) == -1){ 	// if there is a failure
			fprintf(stderr, "input redirect fail\n");
			exit(1);
		}
		else{dup2(inDescript, 0);}
	
	}	
	if(opts->redirectOut == 1){					// if ouput redirection is requested
		outDescript = open(opts->writeTo, O_WRONLY | O_CREAT | O_TRUNC,  0777);
		if(outDescript == -1 || dup2(outDescript,1) == -1){	// set output file descriptor permissions
			fprintf(stderr, "output redirect fail\n");	// if there is a failure
			exit(1);
		}

	}
	execvp(argList[0], argList);		// execute system command from array

	// if the process is not terminated by exec there was an error
	fprintf(stderr, "%s: no such file or directory\n", argList[0]);
	exit(1);

}


//////////////////////////////////////////////////////////////////////////
// Manages signal processing for foreground processes
void foreground(pid_t id, char *exitMessage){

	int state; // holds the state of the process signal
	
	waitpid(id, &state, 0);	// sait for process signal
	
	if(WEXITSTATUS(state)){	// if there was a filure on the exit
		sprintf(exitMessage, "exit value: %d", WEXITSTATUS(state));
		fflush(stdout);
	}
				
	if(WIFSIGNALED(state)){ // if there was a signal sent by process
		sprintf(exitMessage, "terminated by signal %d", WTERMSIG(state)); // display which signal
		printf("%s\n", exitMessage);
	}

	else{ sprintf(exitMessage, "exit value: %d", WEXITSTATUS(state));} // otherwise print exit value	


}
//////////////////////////////////////////////////////////////////////////
// Manages background processes and terminates zombies
void killBackground(){

	pid_t id;	// process id variable
	int state;	// state of the process

	id = waitpid(-1, &state, WNOHANG);	// if any child has exited, but dont wait if none has
	if(id > 0){ // if there was an exited child
		if(WIFEXITED(state)){	// print the process id and exit value of the process
			printf("background pid %d is done: exit value %d\n", id, WEXITSTATUS(state));
			fflush(stdout);
		}
		else if(WIFSIGNALED(state)){ // if the bacground process was signaled to terminate
			// print the process id and the signal that terminated the process
			printf("background pid %d is done: terminated by signal %d\n", id, WTERMSIG(state));
			fflush(stdout);		
		}
	}
}





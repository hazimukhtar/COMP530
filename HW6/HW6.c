/*
Hazim Mukhtar
ONYEN: Hazimukh
Honor Pledge: I certify that no unauthorized assistance has been received or given in the completion of this work.

This program will take in input from the user and then pass the input along as threads that will continuously run 
unlocking and locking with semaphore to check the condition. This will implement memory blocks and then use those memory states
inorder to run each process.  This is similar to hw4 in terms of pipelining.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>  // mmap, munmap
#include <unistd.h>    // sleep
#include <sys/types.h> // kill
#include <signal.h>    // kill
#include <sys/wait.h>
#include "buffer.h"

#define ERROR -1
#define SLEEP_TIME 1


//This is a structure that holds an input and output buffer named sharedBuffers
typedef struct
{
	Buffer* in; // Input buffer
	Buffer* out; // Output buffer
} states;
//definition of methods
Buffer* create_MMAP(size_t size);
void deleteMMAP(void*);

pid_t forkChild(void (*function)(states*), states* state);
void waitForChildren(pid_t*);

void inputer(states* state); 
void removeAst(states* state);
void removeNewLine(states* state);
void output(states* state);
int main(void)
{
	Buffer* input = create_MMAP(sizeof(Buffer)); // Input buffer from stdin
	Buffer* ast= create_MMAP(sizeof(Buffer)); //Buffer to replace ** with ^
	Buffer* newline= create_MMAP(sizeof(Buffer)); // Buffer to replace '\n' with ' '

	createBuffer(input); //Initialize input buffer
	createBuffer(ast); // Initialize ast buffer
	createBuffer(newline); //Initialize newline buffer


	states inputBuff ={ // create input buffer with only an output since the input is user generated
		NULL,
		input
	};

	states astBuff ={ // create a shared buffer with the input and output the converted ** to ^

		input,
		ast
	};
	states nwBuff ={ // create a shared buffer with the ^ and replace '\n' with ' '

		ast,
		newline
	};
	states outBuff ={ //create an output buffer with only an input from newline with output going to stdout

		newline,
		NULL
	};


	//4 processes to be created
	pid_t children[4];


	//each process id is sent to forkChild to run the method
	children[0] = forkChild(inputer, &inputBuff );
        children[1] = forkChild(removeAst, &astBuff );
        children[2] = forkChild(removeNewLine, &nwBuff );
        children[3] = forkChild(output, &outBuff );
	waitForChildren(children);
	
	//delete each memory map
	deleteMMAP(input);
	deleteMMAP(ast);
	deleteMMAP(newline);
	return 0;
}

//This creates a memory map for the Buffer
Buffer* create_MMAP(size_t size){
  //These are the neccessary arguments for mmap. See man mmap.
  void* addr = 0;
  int protections = PROT_READ|PROT_WRITE; //can read and write
  int flags = MAP_SHARED|MAP_ANONYMOUS; //shared b/w procs & not mapped to a file
  int fd = -1; //We could make it map to a file as well but here it is not needed.
  off_t offset = 0;
    
  //Create memory map
  Buffer* state =  mmap(addr, size, protections, flags, fd, offset);
    
  if (( void *) ERROR == state){//on an error mmap returns void* -1.
	perror("error with mmap");
	exit(EXIT_FAILURE);
  }
  return state;
}

void deleteMMAP(void* addr)
{
//This deletes the memory map at given address. see man mmap
	if (ERROR == munmap(addr, sizeof(Buffer))){
		perror("error deleting mmap");
		exit(EXIT_FAILURE);
	}
}


//This functions assigns a fork to the method and the memory state
pid_t forkChild(void (*function)(states*), states* state)
{
     //This function takes a pointer to a function as an argument
     //and the functions argument. It then returns the forked child's pid.

	pid_t childpid;
        switch (childpid = fork()) {
                case ERROR:
                        perror("fork error");
                        exit(EXIT_FAILURE);
                case 0:	
                        (*function)(state);
                default:
                       return childpid;
        }

}


//This method waits for all the children to finish then then kills them.  it tkaes in the PID of the process
void waitForChildren(pid_t* childpids){
	int status;
	while(ERROR < wait(&status)){ //Here the parent waits on any child.
		if(!WIFEXITED(status)){ //If the termination err, kill all children.
			kill(childpids[0], SIGKILL);
	 		kill(childpids[1], SIGKILL);
	 		kill(childpids[2], SIGKILL);
	 		kill(childpids[3], SIGKILL);
			break;
	 	}
	}
}



/*
This method takes in the shared buffer and takes the input from the user and
deposits it onto the input buffer
*/
void inputer(states* state)
{
	while(1)  //run until EOF
	{
		int c = getc(stdin);  //take user input
		deposit(state->out, c); // take user input and put in input buffer
		if(c == EOF) 
		{
			deposit(state->out, EOF); // Send EOF along the buffer chain
			exit(127); // exit the loop
		}
	}
}



/*
This method takes in the sharedBuffers as an argument and removes the ** case
and replaces it with a ^ character.
*/
void removeAst(states* state)
{
	char c;
	while(1)
	{
		c = remoove(state->in); // remove from input buffer
		if(c == EOF)
		{
			deposit(state->out, EOF); // pass EOF along the chain
			exit(127); // exit the loop
		}

		if(c == '*') // if the first char is an *
		{
			char a = remoove(state->in); // check if second char is an *
			if(a == '*')
			{
				deposit(state->out, '^'); // put an * onto the ast buffer
			}
			else
			{
				// place both characters onto the ast buffer
				deposit(state->out, c);
				deposit(state->out, a);
			}
		}
		else
		{
			// place just the original remooved input buffer character onto the ast buffer
			deposit(state->out, c);
		}
	}
}

/*
This method takes in the sharedBuffers ast and newline and removes the '\n' character and replaces it
with the ' ' character
*/
void removeNewLine(states* state)
{
	char c;
	while(1)
	{
		c = remoove(state->in); // remove the character from the ast buffer 
		if(c == EOF)
		{
			deposit(state->out, c); // pass EOF along the chain
			exit(127);
		}

		if(c == '\n') // if the character is a newline character
		{
			deposit(state->out, ' '); // put the ' ' character onto the newline buffer
		}
		else
		{
			deposit(state->out, c); //put the character from the ast buffer onto the newline buffer
		}
	}
	
}

/*
This method takes in the shared buffer of just the newline buffer and prints to stdout a complete 80 character set
*/
void output(states* state)
{
	char c;
	char output[81]; // Initialize the 80 character buffer
	output[80] = '\0'; // Place an ending character at position 80
	int count = 0;
	while(1)
	{
		c = remoove(state->in); // remove the character from the newline buffer
		if(c == EOF)
		{
			exit(127); // break out if EOF was passed along completely
		}
		output[count] = c; // place the character taken onto the output array
		count++; //increase the count by 1
		if(count == 80) // if the array is filled
		{
			printf("%s\n", output); // print out the array of 80 characters
			count = 0; // return count to 0 and start overwritting the exisiting characters
		} 

	}
}

/*
Hazim Mukhtar
ONYEN: Hazimukh
Honor Pledge: I certify that no unauthorized assistance has been received or given in the completion of this work.

This program will take in input from the user and then pass the input along as threads that will continuously run 
unlocking and locking with semaphore to check the condition.  If the thread can pass the semaphore, then it will 
either deposit or remoove a character from the shared buffer.  It will take each character and either change ** into ^ 
or change a newline character into a space character.  Finally, after the final buffer is filled up to 80 characters, 
it will output only the 80 character buffer. 
*/
#include<stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <time.h>
#include <errno.h>
//#include "buffer.c"
#include "buffer.h"
//This is a structure that holds an input and output buffer named sharedBuffers
typedef struct
{
	Buffer* in; // Input buffer
	Buffer* out; // Output buffer
} sharedBuffers;


void *inputer(void* arg); 
void *removeAst(void* arg);
void *removeNewLine(void* arg);
void *output(void* arg);
int main(void)
{
	Buffer input; // Input buffer from stdin
	Buffer ast; //Buffer to replace ** with ^
	Buffer newline; // Buffer to replace '\n' with ' '

	createBuffer(&input); //Initialize input buffer
	createBuffer(&ast); // Initialize ast buffer
	createBuffer(&newline); //Initialize newline buffer


	sharedBuffers inputBuff ={ // create input buffer with only an output since the input is user generated
		NULL,
		&input
	};

	sharedBuffers astBuff ={ // create a shared buffer with the input and output the converted ** to ^

		&input,
		&ast
	};
	sharedBuffers nwBuff ={ // create a shared buffer with the ^ and replace '\n' with ' '

		&ast,
		&newline
	};
	sharedBuffers outBuff ={ //create an output buffer with only an input from newline with output going to stdout

		&newline,
		NULL
	};


	pthread_t t1,t2,t3,t4; // create 4 threads

	// Initialize the 4 threads with the correct shared buffers
	pthread_create(&t1, NULL, inputer, (void*)&inputBuff);
	pthread_create(&t2, NULL, removeAst, (void*)&astBuff);
	pthread_create(&t3, NULL, removeNewLine, (void*)&nwBuff);
	pthread_create(&t4, NULL, output, (void*)&outBuff);

	// wait for all 4 threads to complete before ending
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	pthread_join(t4, NULL);

	return 0;
}

/*
This method takes in the shared buffer and takes the input from the user and
deposits it onto the input buffer
*/
void *inputer(void* arg)
{
	sharedBuffers* buffs = arg; // take the shared buffer argument
	int count = 0; 
	while(1)  //run until EOF
	{
		int c = getc(stdin);  //take user input
		deposit(buffs->out, c); // take user input and put in input buffer
		if(c == EOF) 
		{
			deposit(buffs->out, EOF); // Send EOF along the buffer chain
			break; // exit the loop
		}
	}
	pthread_exit(NULL);
}



/*
This method takes in the sharedBuffers as an argument and removes the ** case
and replaces it with a ^ character.
*/
void *removeAst(void* arg)
{
	sharedBuffers* buffs = arg;  // take the shared buffer, input - ast
	char c;
	while(1)
	{
		c = remoove(buffs->in); // remove from input buffer
		if(c == EOF)
		{
			deposit(buffs->out, EOF); // pass EOF along the chain
			break; // exit the loop
		}

		if(c == '*') // if the first char is an *
		{
			char a = remoove(buffs->in); // check if second char is an *
			if(a == '*')
			{
				deposit(buffs->out, '^'); // put an * onto the ast buffer
			}
			else
			{
				// place both characters onto the ast buffer
				deposit(buffs->out, c);
				deposit(buffs->out, a);
			}
		}
		else
		{
			// place just the original remooved input buffer character onto the ast buffer
			deposit(buffs->out, c);
		}
	}
	pthread_exit(NULL); // exit thread
}

/*
This method takes in the sharedBuffers ast and newline and removes the '\n' character and replaces it
with the ' ' character
*/
void *removeNewLine(void* arg)
{
	sharedBuffers* buffs = arg; // take in the ast and newline buffer
	char c;
	while(1)
	{
		c = remoove(buffs->in); // remove the character from the ast buffer 
		if(c == EOF)
		{
			deposit(buffs->out, c); // pass EOF along the chain
			break;
		}

		if(c == '\n') // if the character is a newline character
		{
			deposit(buffs->out, ' '); // put the ' ' character onto the newline buffer
		}
		else
		{
			deposit(buffs->out, c); //put the character from the ast buffer onto the newline buffer
		}
	}
	pthread_exit(NULL); // exit the thread

}

/*
This method takes in the shared buffer of just the newline buffer and prints to stdout a complete 80 character set
*/
void *output(void* arg)
{
	sharedBuffers* buffs = arg; // take in the newline buffer
	char c;
	char output[81]; // Initialize the 80 character buffer
	output[80] = '\0'; // Place an ending character at position 80
	int count = 0;
	while(1)
	{
		c = remoove(buffs->in); // remove the character from the newline buffer
		if(c == EOF)
		{
			break; // break out if EOF was passed along completely
		}
		output[count] = c; // place the character taken onto the output array
		count++; //increase the count by 1
		if(count == 80) // if the array is filled
		{
			printf("%s\n", output); // print out the array of 80 characters
			count = 0; // return count to 0 and start overwritting the exisiting characters
		} 

	}
	pthread_exit(NULL); // exit the thread
}

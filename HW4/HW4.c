/*
Hazim Mukhtar
ONYEN: Hazimukh
Honor Pledge: I certify that no unauthorized assistance has been received or given in the completion of this work.

This program will take in input from the user and then pass input along through pipes.  As it moves from pipe to pipe, there is a transformation of the 
information that was given until it is outputed to stdout. The output will be 80 characters in length where all ** are changed to ^ and all 
'\n' is change to an open space ' ' character.

*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
//Predefinition of methods used
void inputer(int output);
void removeAst(int input, int output);
void removeNewLine(int input, int output);
void output(int input);

//This method creates the pipes used for the program to transform the input and print out the output using forks
int main()
{
	int sharedPipes[4][2]; // this is the pipes going to be used
	int count = 0;
	for(count = 0; count < 4; count++) // create the pipes
	{
		if(pipe(sharedPipes[count]) == -1) // check if pipe creation failed
		{
			printf("failed pipe creation");
			exit(EXIT_FAILURE);
		}
	}

	pid_t pids[4]; // the 4 forks that will need to be run, their PID

	for(count = 0; count < 4; count++)
	{
		pids[count] = fork(); // fork a process
		if(pids[count] == -1) // check if it failed
		{
			printf("failed forking");
			exit(EXIT_FAILURE);
		}
		else if(pids[count] == 0) // if it did not fail then based on the count, do one of the commands
		{
			if(count == 0) // input
				inputer(sharedPipes[0][1]);
			if(count == 1)// remove ** to ^
				removeAst(sharedPipes[count-1][0], sharedPipes[count][1]);
			if(count == 2) // remove new line characters '\n' to ' '
				removeNewLine(sharedPipes[count-1][0], sharedPipes[count][1]);
			if(count == 3)// output the 80 lines
				output(sharedPipes[count-1][0]);
			exit(EXIT_SUCCESS);
		}


	}

	while(count > 0) // wait for all of the threads to finish
	{
		wait(NULL);
		count--;
	}
 
	return 0;
}

//This pipe takes in the output pipe as a paramter to write the input into it. 
void inputer(int output)
{
	int c;
	do
	{
		c = getc(stdin); // get input from typing
		write(output, &c, sizeof(char)); // write input to the output pipe
	} while(c != EOF); // wait until EOF is reached
}



//This method takes in the input pipe and an output pipe.  The input pipe was the output pipe of the inputer method. 
void removeAst(int input, int output)
{
	char c;
	while(1)
	{
		read(input, &c, sizeof(char)); // read from the pipe

		if(c == '*') // if * is reached
		{
			char a;
			read(input, &a, sizeof(char)); // check the next character if it is *
			if(a == '*')
			{
				c = '^';
				write(output, &c, sizeof(char)); // write ^ if ** is reached
			}
			else // write the first and second charactered reached if ** was not found
			{
				if(a == EOF)
				{
					write(output, &c, sizeof(char));
					write(output, &a, sizeof(char));
					return;
				}
				write(output, &c, sizeof(char));
				write(output, &a, sizeof(char));
			}
		}
		else // write the character inputed if * was not it
		{
			if(c == EOF)
			{
				write(output, &c, sizeof(char));
				return;
			}
			write(output, &c, sizeof(char));

		}
	}
}

// This method removes the newline character from the input pipe and puts a ' ' in the output pipe. The input pipe is the output pipe of the ast method
void removeNewLine(int input, int output)
{
	char c;
	do
	{
		read(input, &c, sizeof(char)); // take from the input pipe

		if(c == '\n')
			c = ' ';
		write(output, &c, sizeof(char)); // write ' ' if '\n' is reached
	}while(c != EOF);
}

// This method takes in an input pipe which was the output pipe of the removeNewLine method and prints out the 80 character sequence. 
void output(int input)
{
	char c;
	char output[81];
	output[80] = '\0'; // add the terminator to the end of the 80 characters
	int counter = 0;
	while(1)
	{
		read(input, &c, sizeof(char));
		if(c == EOF)
		{
			return;
		}
		output[counter] = c;
		counter++;
		if(counter == 80) // if 80 characters were read, then output it
		{
			printf("%s\n", output);
			counter = 0; // reset the counter. 
		}

	}
}

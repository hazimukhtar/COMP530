/*
Hazim Mukhtar
ONYEN: Hazimukh
Honor Pledge: I certify that no unauthorized assistance has been received or given in the completion of this work.

This program is a mockup shell that will parse input, take the fork, fork a new process
and then from the child process execute the command given.  If there is a problem, it
will identify the problem and output an error and continue to take in input for the next process.
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
//Method to parse the input string (unused)
//char** parseString();

// Take in commands until EOF
int main()
{

	char* input; // user input
	int childPID = 1; // Starting value of ChildPID
	int args =0; //Count for Number of Arguments
	int count = 0; // Counter for malloc pointers
	char* split =""; // temp Pointer for tokenizing
	char** argv; //Pointer of Pointers for final tokenization

	input = = (char*)malloc(2500*sizeof(char));
	argv = (char**)malloc(2500*sizeof(char*));
	for(count = 0; count < 2500; count++) // Initializing pointer of pointers
		argv[count] = (char*)malloc(2500*sizeof(char));

	while(input != NULL) // Wait for EOF
	{

		printf("%% ");
		input = fgets(input, 2500, stdin); // Take input from stdin up to 2500
		childPID = fork(); // Fork a process for the child

		if(childPID == 0) // Child Process
		{
			split = strtok(input, " \t\n");  // Tokenize at ' ', '\t', '\n'
			while (split != NULL)
			{
				strcpy(argv[args++], split);  // Take tokens and put into argument pointer
				split = strtok(NULL, " \t\n"); // repeat the tokenization
			}

			argv[args++] = NULL; // Add the required NULL Pointer at the end
			execvp(argv[0],argv); // Execute the command 
			fprintf(stderr,"Command Not Found\n"); // Error Handling for commands not found
			exit(127); // Exit if Successful
		}
		else // Parent Process
		{
			wait(NULL); // Wait for the child to finish 
		}
	}
	return 0;
}


//Method to parse the input of the user and create a new pointer
//of pointers and return it to the main method.  
//Input: array of characters (input[])
//Return Value: Pointer of Pointers of Characters (finalPt)
/*
char** parseString(char input[])
{
	int count = 0;
	int tempCount = 0;
	int inputLength = 0;
	int tempLength = 0;
	int arg = 0;
	char tempString[100];
	static char *temp[10];
	for (count=0; count<10; count++)
		temp[count] = (char*)malloc(100 * sizeof(char));

	inputLength = strlen(input);

	for(count=0; count<inputLength; count++)
	{
		if(input[count] == ' ' || input[count] == '\n' || input[count] == '\t')
		{
			tempString[tempCount] = '\0';

			strcpy(temp[arg], tempString);
			memset(tempString, 0, sizeof(tempString));
			arg++;
			tempCount=0;
			count++;

		}
		tempString[tempCount] = input[count];
		tempCount++;
	}

	char** finalPt = (char**)malloc(arg * sizeof(char *));
	for (count=0; count<arg; count++)
		finalPt[count] = (char*)malloc(100 * sizeof(char));

	for (count = 0; count < arg; count++)
	{
		strcpy(finalPt[count] , temp[count]);
	}

	finalPt[arg] = NULL;

	return finalPt;
}
*/

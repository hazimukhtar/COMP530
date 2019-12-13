/*
Hazim Mukhtar
ONYEN: Hazimukh
Honor Pledge: I certify that no unauthorized assistance has been received or given in the completion of this work.

This is the serverside of the distributed shell.  This program takes the input from the client and then reads it in throught the socket and runs the commands using 
execvp.  From there, it is able to then take the output put it in a file and then read the file and transfer the output over the socket to the client where it is printed. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "Socket.h"
/* variables to hold socket descriptors */
  ServerSocket welcome_socket;
  Socket connect_socket;

void execCommands(void);
// take the arguments of the port number
int main(int argc, char* argv[])
{
  	pid_t spid; /* pid_t is typedef for Linux process ID */ 
  	pid_t temp;
	int chld_status ;
	
  	if (argc < 2)
     	{
      		printf("No port specified\n");
      		return (-1);
     	}
  	welcome_socket = ServerSocket_new(atoi(argv[1]));  // create the port socket based on input
  	if (welcome_socket < 0)
      	{
      		printf("Failed new server socket\n");
      		return (-1);
     	}

	while (1)
    	{
     		connect_socket = ServerSocket_accept(welcome_socket); //create the socket to transfer the data
     		if (connect_socket < 0)
        	{
        	 	printf("Failed accept on server socket\n");
        	 	exit (-1);
        	}
     		spid = fork();  /* create child == service process */
     		if (spid < 0) 
        	{
        	 	perror("fork"); 
        	 	exit (-1);
        	}
     		else if (spid == 0) // child process
       		{
		 	execCommands();  //if child then run the exec Function
			printf("exit");
        	 	Socket_close(connect_socket);  //close the socket
        	 	exit(127);
        	}
		else
        	{
        	 	Socket_close(connect_socket); // close the socket
        	 	temp = waitpid(-1, &chld_status, 0); //wait for the child to be done
			exit(0);
        	}
    	}
	return 0;
}

void execCommands()
{
	char c; // character temp
	int args = 0; //number of arguments
	int count = 0; //count for the args
	char* input; // input
	char* split = ""; // the arguments split based on delimiter
	char ** argv; // the array of arrays holding the arguments
	input = (char*)malloc(2500*sizeof(char));  //initialize the input
        argv = (char**)malloc(2500*sizeof(char*)); //initialize rows of the matrix
	for(count = 0; count < 2500; count++) // Initializing pointer of pointe$
                argv[count] = (char*)malloc(2500*sizeof(char));

  	Socket_close(welcome_socket); //close the welcome socket
	char tmp[100]; //create array for file name
	snprintf(tmp, 100, "temp%d", getpid()); // save the name for the tmp file created

  	while(input != NULL) // if input != EOF
	{

		pid_t childPid = fork();  //create a child process
		if(childPid < 0)
		{
			perror("fork 2\n");
			exit(EXIT_FAILURE);
		}
		else if(childPid == 0)
		{
			freopen(tmp, "w", stdout); //send all the output to the file
			freopen(tmp, "w", stderr);
			for(count = 0; count < 2500; count++)
			{
				c = Socket_getc(connect_socket); //get information from the client one letter at a time
				if(c == EOF)
				{
					printf("Socket_getc EOF or error. Exit.");
					return;
				}
				input[count] = c; // put the information into input
				if(c == '\0')
				break;
			}



			
			split = strtok(input, " \t\n");
			//This is used to split the string at the appropriate delimiters
			while(split!=NULL)
			{
				strcpy(argv[args++], split);
				split = strtok(NULL, " \t\n");
				if(split == NULL)
				{
					break;
				}
			}
			argv[args++] = NULL; // Add the required NULL at the end
			execvp(argv[0],argv); // Execute the commmand
	              	printf("failed to execute command\n"); //Print if failed
			exit(127); // Exit if Successful

		}
		else
		{

			waitpid(childPid, NULL, 0); //wait for the child to finish
			printf("done waiting for command\n"); //tell us it finished
			char output;
			FILE* fp = fopen(tmp, "r"); //open the file to be read
			while((output = fgetc(fp)) != EOF)
			{
				 // get it character by character from the file
				if(output == EOF)
				{
					printf("Socket_putc EOF or error. Exit");
					remove(tmp);
					return;
				}
				Socket_putc(output, connect_socket); // put it back on the connection socket ot be read from the client

			}
			Socket_putc('\0', connect_socket);
//			printf("outide the loop");
			fclose(fp); //close the file
			remove(tmp); // remove the temporary file
		}

	}
}



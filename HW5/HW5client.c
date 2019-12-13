/*
Hazim Mukhtar
ONYEN: Hazimukh
Honor Pledge: I certify that no unauthorized assistance has been received or given in the completion of this work.

This program is a distributed shell.  This side is the client side.  It connects to the open port from the specificied server and then sends characters overa socket.  The response from the server, which 
executes the commands from the client then sends over the response and is prinited out from here. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Socket.h"
// Take in the arguments of the server address and the port number
int main(int argc, char* argv[])
{
  int i, c, rc;
  int count = 0;
  int length = 0;

  char* line;
  line = (char*)malloc(2500*sizeof(char));  //create the input and output for sending and receiving over the socket

  /* variable to hold socket descriptor */
  Socket connect_socket;

  if (argc < 3)
     {
      printf("No host and port\n");
      return (-1);
     }
		printf("%% ");

  /* connect to the server at the specified host and port;
   * blocks the process until the connection is accepted
   * by the server; creates a new data transfer socket.
   */
	  connect_socket = Socket_new(argv[1], atoi(argv[2]));
  	if (connect_socket < 0)
      	{
       		printf("Failed to connect to server\n");
       		return 0;
      	}
	
	while (fgets(line, 2500, stdin) != NULL)  //take in user input
     	{
		if(line == NULL)
			break;
		length = strlen(line) + 1; //add one more space for the ending space
		for(count = 0; count < length; count++)
		{
			if(Socket_putc((int) line[count], connect_socket) == EOF) // send over the socket until EOF
			{
				printf("failed to send character");
				exit(-1);
			}
		}

		char output;
		for(count = 0; count < 2500; count++) // This is for taking back the input from the sokcet
		{
			output = Socket_getc(connect_socket); // This is the output from the server
			if(output == EOF)
			{
				printf("Socket_getc EOF or error\n");
				Socket_close(connect_socket);
				exit(-1);
			}
			line[count] = output;
			if(line[count] == '\0') // wait until the end character is read
				break;
		}
		if( count == 2500)
			line[count-1] = '\0'; // add an ending character if it reaches the max size
		printf("%s", line);  // print out the output.
		printf("%% ");

	}

	Socket_close(connect_socket); // close the socket and exit. 
	return 0;
}

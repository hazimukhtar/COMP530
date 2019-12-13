#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include "buffer.h"

/*
This method takes in a buffer and initializes its variable values
*/
void createBuffer(Buffer* buff)
{
	buff -> in = 0; // set the in value to 0
	buff -> out = 0; // set the out value to 0
	sem_init(&buff->emptySem, 1, 200); // set the semaphore for the empty semaphore to 1
	sem_init(&buff->fullSem,1, 0); // set the full semaphore to the value of 0

}


/*
This method takes in a character and a buffer and puts the character
at the buff->in location 
*/
void deposit(Buffer* buff, char c)
{
	sem_wait(&buff->emptySem); // This makes the empty semaphore go down by 1 since we are depositing a character
	buff->stream[buff->in] = c; // place the character in the in location of the buff
	buff->in = (buff->in +1) % 200; // increase the in location by one and mod by 20 to make it circular
	sem_post(&buff->fullSem); // This increases the full semaphore by 1 since it has a new character
}

/*
This method takes in a buffer and removes the character returing that character 
from the method
*/
char remoove(Buffer* buff)
{

	sem_wait(&buff->fullSem); // This increases the full semaphore by 1 with the removal of a character
	char temp = buff->stream[buff->out]; // This keeps the value in the out location of the buffer
	buff->out = (buff->out+1)%200; // this increases the value of the out and mod 20 to keep the count going around the buffer
	sem_post(&buff->emptySem); // this increases the empty semaphore by 1 since a value was removed and another character can be added to the buffer
	return temp; // This returns the character taken from the buffer

}

/*
This method takes in a buffer and prints all of its values.  Used for debugging
*/
void print(Buffer* buff)
{
	int count = 0;
	for(count = 0; count < 201; count++)
	{
		printf("%c", buff->stream[count]); // This goes through each value in the buffer and prints each one
	}
	printf("\n");
}

/*
This is a header file for a circular buffer that takes in
a character and outputs a character
*/

#ifndef _buffer_h_
#define _buffer_h_

#include <semaphore.h>


typedef struct buffer
{
	char stream[200]; // a character array of 20 characters
	int in; // The next location to input the char
	int out; // The next location to output a char
	sem_t emptySem; // Semaphore for the empty buffer condition
	sem_t fullSem;  // Semaphore for the full buffer condition


}Buffer;

/*
This is a method that will take in the buffer specified and initialize its variables
*/
extern void createBuffer(Buffer* buff);


/*
This is a method that will take in a buffer and a character 
and input the character at the buff->in variable location
*/
extern void deposit(Buffer* buff, char c);

/*
This is a method that will take in a buffer and remove the character
from the buff->out location 
*/
extern char remoove(Buffer* buff);

/*
This is a method that will take in a buffer and output the values
of the specified buffer
*/
extern void print(Buffer* buff);
#endif

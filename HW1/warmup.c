#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
	char input;
	char output[81]; 
	int count = 0;
	int c;
	c = getc(stdin);
	int d;
	output[80] = '\0';
//	printf("-------------------------------------------------------------------------------|\n");
	while (c != EOF)
	{
		if (c == '\n')
			output[count] = ' ';

		else if (c == '*')
		{
			d = getc(stdin);
			if (d == '*')
				output[count] = '^';
			else
			{
				ungetc(d,stdin);
				output[count] = c;
			}
		}


		else
		{
			output[count] = c;
		}
		c = getc(stdin);
		count++;
		if (count == 80)
		{
			printf("%s\n", output);
			count = 0;
		}

	}

	return 0;
}


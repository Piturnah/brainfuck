// Example usage: `gcc ./main.c && mv a.out brain`

//MIT License

//Copyright (c) 2022 Peter Hebden

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define memCapacity 30000
int mem[memCapacity];

#define stackCapacity 1024
char* retStack[stackCapacity];
int retStackSize = 0;

void usage()
{
    printf("USAGE: ./brain <program>\n");
    printf("FLAGS\n");
    printf("    -d - enters debug mode\n");
    printf("    -s - enters step mode\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
	fprintf(stderr, "ERROR: NO PROGRAM SOURCE PROVIDED\n");
	usage();
	return 1;
    }

    FILE *fptr;
    if ((fptr = fopen(argv[1], "r")) == NULL)
    {
	fprintf(stderr, "ERROR: Could not open file %s\n", argv[1]);
	usage();
	return 1;
    }

    int debug = 0;
    int step = 0;
    for (int i = 2; i < argc; i++)
    {
	if (strcmp(argv[i], "-d") == 0)
	{
	    debug = 1;
	}
	else if (strcmp(argv[i], "-s") == 0)
	{
	    step = 1;
	}
	else
	{
	    fprintf(stderr, "ERROR: Didn't recognise argument `%s`\n", argv[i]);
	    usage();
	    return 1;
	}
    }

    int dataPtr = 0;

    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    char *program = malloc(fsize + 1);
    fread(program, fsize, 1, fptr);
    fclose(fptr);

    program[fsize] = 0;

    while (*program != 0)
    {
	switch(*program++)
	{
	case (int)'+':
	    mem[dataPtr]++;
	    break;
	case (int)'-':
	    mem[dataPtr]--;
	    break;
	case (int)'>':
	    dataPtr++;
	    break;
	case (int)'<':
	    dataPtr--;
	    break;
	case (int)'.':
	    printf("%c", mem[dataPtr]);
	    break;
	case (int)',':
	    mem[dataPtr] = getchar();
	    break;
	case (int)'[':
	    if (mem[dataPtr] == 0)
	    {
		int stackSize = 1;
		while (stackSize != 0)
		{
		    switch(*program++)
		    {
		    case (int)'[':
			stackSize++;
			break;
		    case (int)']':
			stackSize--;
			break;
		    }
		}
	    }
	    else
	    {
		retStack[retStackSize++] = program - 1;
	    }
	    break;
	case (int)']':
	    program = retStack[--retStackSize];
	    break;
	}

	if (debug)
	{
	    char ptrHere(int index)
	    {
		if (dataPtr == index)
		{
		    return '^';
		}
		else
		{
		    return ' ';
		}
	    }

	    printf("\n %c to\n", *(program));
	    printf("%2d | %2d | %2d | %2d | %2d | %2d | %2d | %2d | %2d | %2d\n", mem[0], mem[1], mem[2], mem[3], mem[4], mem[5], mem[6], mem[7], mem[8], mem[9]);
	    printf("%2c | %2c | %2c | %2c | %2c | %2c | %2c | %2c | %2c | %2c\n", ptrHere(0), ptrHere(1), ptrHere(2), ptrHere(3), ptrHere(4), ptrHere(5), ptrHere(6), ptrHere(7), ptrHere(8), ptrHere(9));
	}
	if (step)
	{
	    getchar();
	}
    }

    return 0;
}

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
char mem[memCapacity];

#define stackCapacity 1024
char* retStack[stackCapacity];
int retStackSize = 0;

void usage(char *execName)
{
    printf("USAGE: %s <program>\n", execName);
    printf("FLAGS\n");
    printf("    -d <?memcap> - enters debug mode with optional argument for memory capacity\n");
    printf("    -s - enters step mode\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
	fprintf(stderr, "ERROR: NO PROGRAM SOURCE PROVIDED\n");
	usage(argv[0]);
	return 1;
    }

    FILE *fptr;
    if ((fptr = fopen(argv[1], "r")) == NULL)
    {
	fprintf(stderr, "ERROR: Could not open file %s\n", argv[1]);
	usage(argv[0]);
	return 1;
    }

    int debug = 0;
    int debugMemCapacity = 15;
    int step = 0;
    for (int i = 2; i < argc; i++)
    {
	if (strcmp(argv[i], "-d") == 0)
	{
	    debug = 1;
	    if (i + 1 != argc && (debugMemCapacity = atoi(argv[++i])) == 0)
	    {
		debugMemCapacity = 15;
		i--;
	    }
	}
	else if (strcmp(argv[i], "-s") == 0)
	{
	    step = 1;
	}
	else
	{
	    fprintf(stderr, "ERROR: Didn't recognise argument `%s`\n", argv[i]);
	    usage(argv[0]);
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

	    printf("\n %c to\n", *(program-1));

	    char memOutBuff[debugMemCapacity * 5 + 1], pointerLocBuff[debugMemCapacity * 5 + 1];
	    // TODO: investigate safety of this
	    memset(memOutBuff, 0, debugMemCapacity * 5 + 2);
	    memset(pointerLocBuff, 0, debugMemCapacity * 5 + 2);
	    for (int i = 0; i < debugMemCapacity; i++)
	    {
		sprintf(&memOutBuff[i * 5], "|%3d ", mem[i]);
		sprintf(&pointerLocBuff[i * 5], "| %2c ", ptrHere(i));
	    }
	    memOutBuff[debugMemCapacity * 5] = '|';
	    pointerLocBuff[debugMemCapacity * 5] = '|';
	    printf("%s\n%s\n", memOutBuff, pointerLocBuff);
	}
	if (step)
	{
	    getchar();
	}
    }

    return 0;
}

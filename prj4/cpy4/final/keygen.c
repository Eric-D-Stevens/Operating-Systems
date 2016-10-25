#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{	
	int size;
	char let;
	int i;

	if(argc < 2)
	{
		printf("usage: keygen <size>\n");
		exit(1);
	}
	
	sscanf(argv[1], "%d", &size);
	
	srand((int)time(NULL));

	for(i = 0; i < size; i++)
	{
		let = (char)((rand()%(90-63))+64);
		if(let == '@'){let = ' ';}
		printf("%c", let);

	}

	printf("\n");
	return 0;
}

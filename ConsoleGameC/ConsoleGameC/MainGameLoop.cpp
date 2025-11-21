#include <stdio.h>
#include <stdlib.h>
#include <conio.h>//use to get input without waiting for enter

#define MAP_SIZE 1350
#define UNDER_MAP "\033[H\033[16B"
#define HOME "\033[H"
#define SPACES_TO_REACH_TOP 16//doesnt go to the top one for some reason
#define SPACES_TO_REACH_RIGHT 31




int main(void)
{
	system("chcp 65001 > nul");


	FILE* mapFile;
	fopen_s(&mapFile, "../../map.txt", "r");
	if (mapFile == NULL)
	{
		perror("ERROR");
		return EXIT_FAILURE;
	}

	char mapString[MAP_SIZE] = { 0 };
	fread_s(mapString, MAP_SIZE, 1, MAP_SIZE, mapFile);
	printf("%s\n", mapString);

	int u = 8;
	int d = 0;//unused
	int r = 16;
	int l = 0;//unused

	while (true)
	{
		//print the map
		printf(HOME);
		printf("%s", mapString);

		//move to current position and print player
		printf(UNDER_MAP);
		printf("\033[%dA\033[%dB\033[%dC\033[%dD", u, d, r, l);
		printf("%c", 254);

		//get input and print it under the map
		char input = _getch();
		printf(UNDER_MAP);
		printf("%c", input);

		//get movement option
		switch (input)
		{
		case 'w'://up
			if (u <= SPACES_TO_REACH_TOP)
			{
				u++;
			}
			break;
		case 's'://down
			if (u > 0)
			{
				u--;
			}
			break;
		case 'a'://left
			if (r > 0)
			{
				r--;
			}
			break;
		case 'd'://right
			if (r <= SPACES_TO_REACH_RIGHT)
			{/*
				printf("\033[%dA\033[%dB\033[%dC\033[%dD", u, d, r + 1, l);
				if (getc(stdin) != 219)
				{
					r++;
				}
				ungetc(219, stdin);*/
				r++;
			}
			break;
		}
	}
}
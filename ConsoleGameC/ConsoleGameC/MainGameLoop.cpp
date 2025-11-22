#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//map measurements
#define MAP_SIZE 1500
#define MAP_HEIGHT 16
#define MAP_WIDTH_PLUS1 33//add extra character for \0
#define MAP1_FILE "Maps/map1.txt"
#define SPACES_TO_REACH_BOTTOM 16
#define SPACES_TO_REACH_RIGHT 32

//sleep amounts
#define WALK_SLEEP 200
#define SPRINT_SLEEP 100

//keys
#define W_KEY 0x57
#define S_KEY 0x53
#define A_KEY 0x41
#define D_KEY 0x44
#define P_KEY 0x50

//cursor strings
#define CURSOR_UNDER_MAP "\033[H\033[17B"
#define CURSOR_HOME "\033[H"
#define CURSOR_HIDE "\033[?25l"
#define CURSOR_SHOW "\033[?25h"
#define CLEAR_SCREEN "\033[2J"

//character codes
#define WALL_CHARACTER_VERTICAL 186



int main(void)
{
	printf(CURSOR_HIDE);

	FILE* mapFile;
	fopen_s(&mapFile, MAP1_FILE, "r");
	if (mapFile == NULL)
	{
		perror("ERROR");
		return EXIT_FAILURE;
	}

	unsigned char mapString[MAP_SIZE] = { 0 };
	unsigned char tempMapString[MAP_SIZE] = { 0 };
	fread_s(mapString, MAP_SIZE, 1, MAP_SIZE, mapFile);
	strncpy_s((char*)tempMapString, MAP_SIZE, (char*)mapString, MAP_SIZE);

	//use this to track where collision objects are
	unsigned char mapTracker[MAP_HEIGHT][MAP_WIDTH_PLUS1] = { { 0 } };

	//copy map to collision tracker
	char* nextLine = NULL;
	char* line = strtok_s((char*)tempMapString, "\n", &nextLine);
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		strncpy_s((char*)mapTracker[i], MAP_WIDTH_PLUS1, line, MAP_WIDTH_PLUS1);
		mapTracker[i][MAP_WIDTH_PLUS1 - 1] = '\0';
		line = strtok_s(NULL, "\n", &nextLine);
	}


	//starting position
	int u = 0;//unused
	int d = 8;
	int r = 10;
	int l = 0;//unused

	bool doLoop = true;
	int sleepSprint = SPRINT_SLEEP;
	//by default skip sprint to make key presses more responsive
	//only do the sprint if player is moving
	bool skipSprint = true;

	while (doLoop)
	{
		//print the map
		printf(CURSOR_HOME);
		printf("\n%s", mapString);

		//move to current position and print player
		printf(CURSOR_HOME);
		printf("\033[%dA\033[%dB\033[%dC\033[%dD", u, d, r, l);
		printf("%c", 254);

		//get input and set position
		//up/down and left/right are 1 indexed
		
		//up
		if (GetAsyncKeyState(W_KEY) & 0x8000)
		{
			if (mapTracker[d - 2][r - 1] != WALL_CHARACTER_VERTICAL)
			{
				if (d > 1)
				{
					d--;
					skipSprint = false;
				}
			}
		}
		//down
		if (GetAsyncKeyState(S_KEY) & 0x8000)
		{
			if (mapTracker[d][r - 1] != WALL_CHARACTER_VERTICAL)
			{
				if (d < SPACES_TO_REACH_BOTTOM)
				{
					d++;
					skipSprint = false;
				}
			}
		}
		//left
		if (GetAsyncKeyState(A_KEY) & 0x8000)
		{
			if (mapTracker[d - 1][r - 2] != WALL_CHARACTER_VERTICAL)
			{
				if (r > 1)
				{
					r--;
					skipSprint = false;
				}
			}
		}
		//right
		if (GetAsyncKeyState(D_KEY) & 0x8000)
		{
			if (mapTracker[d - 1][r] != WALL_CHARACTER_VERTICAL)
			{
				if (r <= SPACES_TO_REACH_RIGHT)
				{
					r++;
					skipSprint = false;
				}
			}
		}
		//sprint
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			sleepSprint = SPRINT_SLEEP;
			skipSprint = false;
		}
		else
		{
			sleepSprint = WALK_SLEEP;
		}
		//exit
		if (GetAsyncKeyState(P_KEY) & 0x0001)
		{
			doLoop = false;
		}

		//debug
		{
			printf(CURSOR_UNDER_MAP);
			//extra space to clear old values if 1 digit
			printf("d=%d r=%d       ",d, r);
		}
		//end of debug

		if (!skipSprint)
		{
			Sleep(sleepSprint);
		}
		else
		{
			Sleep(8);
		}
		skipSprint = true;
	}//end of while loop

	printf(CLEAR_SCREEN CURSOR_HOME);
}
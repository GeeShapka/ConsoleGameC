#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//map measurements
#define MAP_SIZE 1500
#define MAP_HEIGHT 16
#define MAP_WIDTH 35
#define MAP1_FILE "../../map1.txt"
#define SPACES_TO_REACH_TOP 16
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

//character codes
#define WALL_CHARACTER_LEFTRIGHT 186



int main(void)
{
	FILE* mapFile;
	fopen_s(&mapFile, MAP1_FILE, "r");
	if (mapFile == NULL)
	{
		perror("ERROR");
		return EXIT_FAILURE;
	}

	char mapString[MAP_SIZE] = { 0 };
	fread_s(mapString, MAP_SIZE, 1, MAP_SIZE, mapFile);

	//use this to track where collision objects are
	unsigned char mapTracker[MAP_HEIGHT][MAP_WIDTH] = { { 0 } };
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		sscanf_s(mapString, "%[^\n]", mapTracker[i], MAP_WIDTH);
	}
	//printf("%s\n", mapString);

	int u = 8;
	int d = 0;//unused
	int r = 16;
	int l = 0;//unused

	bool doLoop = true;
	int sleepSprint = 200;
	bool skipSleep = false;
	while (doLoop)
	{
		//print the map
		printf(CURSOR_HOME);
		printf("\n%s", mapString);

		//move to current position and print player
		printf(CURSOR_UNDER_MAP);
		printf("\033[%dA\033[%dB\033[%dC\033[%dD", u, d, r, l);
		printf("%c", 254);

		//get input and set position
		//up/down is 1 indexed
		//left/right is 0 indexed
		
		//up
		if (GetAsyncKeyState(W_KEY) & 0x8000)
		{
			if (u <= SPACES_TO_REACH_TOP)
			{
				u++;
				skipSleep = false;
			}
		}
		//down
		if (GetAsyncKeyState(S_KEY) & 0x8000)
		{
			if (u > 1)
			{
				u--;
				skipSleep = false;
			}
		}
		//left
		if (GetAsyncKeyState(A_KEY) & 0x8000)
		{
			if (mapTracker[u][r - 2] != WALL_CHARACTER_LEFTRIGHT)
			{
				if (r > 1)
				{
					r--;
					skipSleep = false;
				}
			}
		}
		//right
		if (GetAsyncKeyState(D_KEY) & 0x8000)
		{
			if (mapTracker[u][r] != WALL_CHARACTER_LEFTRIGHT)
			{
				if (r <= SPACES_TO_REACH_RIGHT)
				{
					r++;
					skipSleep = false;
				}
			}
		}
		//sprint
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			sleepSprint = SPRINT_SLEEP;
			skipSleep = false;
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
		printf(CURSOR_UNDER_MAP);
		printf("u=%d r=%d",u, d);

		if (!skipSleep)
		{
			Sleep(sleepSprint);
		}
		else
		{
			Sleep(8);
		}
		skipSleep = true;
	}//end of while loop
}
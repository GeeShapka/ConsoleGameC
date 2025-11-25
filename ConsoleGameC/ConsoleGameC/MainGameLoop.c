#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <errno.h>

//map measurements
int MAP_SIZE = 0;
#define MAP_HEIGHT 16
#define MAP_WIDTH 32
#define MAP_WIDTH_PLUS1 33//add extra character for \0
#define SPACES_TO_REACH_BOTTOM 16
#define SPACES_TO_REACH_RIGHT 32

//map paths
#define MAP1_FILE "Maps/map1.txt"
#define MAP1COLLISION_FILE "Maps/map1Collision.txt"

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

//error strings
#define ERROR_OBTAINING_MAP_STRING "ERROR: could not obtain map string\n"

//character codes
#define WALL_CHARACTER_VERTICAL 186



//function declarations
int setMapSize(int height, int width);
unsigned char* getMapString(const char* mapPath, size_t mapSize);
unsigned char** getCollisionTracker(size_t height, size_t width, unsigned char* collisionMapString);





//main
int main(void)
{
	//set the expected size of each map
	MAP_SIZE = setMapSize(MAP_HEIGHT, MAP_WIDTH);

	printf(CURSOR_HIDE);

	//open map
	unsigned char* mapString = getMapString(MAP1_FILE, MAP_SIZE);

	//get the collision tracker
	unsigned char** collisionTracker = getCollisionTracker(MAP_HEIGHT, MAP_WIDTH, getMapString(MAP1COLLISION_FILE, MAP_SIZE));

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

	//main loop
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
			if (d > 1)
			{
				if (collisionTracker[d - 2][r - 1] != WALL_CHARACTER_VERTICAL)
				{
					d--;
					skipSprint = false;
				}
			}
		}
		//down
		if (GetAsyncKeyState(S_KEY) & 0x8000)
		{
			if (d < SPACES_TO_REACH_BOTTOM)
			{
				if (collisionTracker[d][r - 1] != WALL_CHARACTER_VERTICAL)
				{
					d++;
					skipSprint = false;
				}
			}
		}
		//left
		if (GetAsyncKeyState(A_KEY) & 0x8000)
		{
			if (r > 1)
			{
				if (collisionTracker[d - 1][r - 2] != WALL_CHARACTER_VERTICAL)
				{
					r--;
					skipSprint = false;
				}
			}
		}
		//right
		if (GetAsyncKeyState(D_KEY) & 0x8000)
		{
			if (r <= SPACES_TO_REACH_RIGHT)
			{
				if (collisionTracker[d - 1][r] != WALL_CHARACTER_VERTICAL)
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



/// <summary>
/// returns the cound of the characters in the mapFiles
/// while accounting for the \n character
/// </summary>
/// <param name="height"></param>
/// <param name="width"></param>
/// <returns></returns>
int setMapSize(int height, int width)
{
	return((height * width) + (height - 1));
}



/// <summary>
/// gets a map string from a file
/// </summary>
/// <param name="mapPath">file path of the file</param>
/// <param name="mapSize">size in bytes/num of characters of the string</param>
/// <returns>a pointer to the string</returns>
unsigned char* getMapString(const char* mapPath, size_t mapSize)
{
	//open the file
	FILE* mapFile;
	fopen_s(&mapFile, MAP1_FILE, "r");
	if (mapFile == NULL)
	{
		perror("Could not open map file");
		exit(EXIT_FAILURE);
	}

	//allocate space for map with extra space for \0
	unsigned char* map = (unsigned char*)calloc(mapSize + 1, sizeof(char));
	if (map == NULL)
	{
		perror("Could not allocat memory for a map\n");
		exit(EXIT_FAILURE);
	}

	//if no bytes read
	if (fread_s(map, mapSize, sizeof(char), mapSize, mapFile) == 0)
	{
		perror("Could not read from a map file\n");
		exit(EXIT_FAILURE);
	}
	map[mapSize] = '\0';

	//close the file
	if (fclose(mapFile) == -1)
	{
		perror("ERROR: ");
		exit(EXIT_FAILURE);
	}
	
	//return a pointer to the space where the map is
	return(map);
}//end of getMapString



/// <summary>
/// gets the 2d array for a collision tracker
/// </summary>
/// <param name="height"></param>
/// <param name="width"></param>
/// <param name="collisionMapString"></param>
/// <returns>the char** to the array</returns>
unsigned char** getCollisionTracker(size_t height, size_t width, unsigned char* collisionMapString)
{
	//allocate the outer array
	unsigned char** collisionTracker = (unsigned char**)calloc(height, sizeof(char*));
	if (collisionTracker == NULL)
	{
		perror("Could not allocate collision tracker");
		exit(EXIT_FAILURE);
	}

	//allocate the inner arrays
	for (int i = 0; i < height; i++)
	{
		collisionTracker[i] = (unsigned char*)calloc(width + 1, sizeof(char));
		if (collisionTracker[i] == NULL)
		{
			perror("Could not allocate inner array in collision tracker");
			exit(EXIT_FAILURE);
		}
	}

	//copy each line from the map string into the inner arrays
	char* nextLine = NULL;
	char* line = strtok_s((char*)collisionMapString, "\n", &nextLine);
	for (int i = 0; i < height; i++)
	{
		strncpy_s((char*)collisionTracker[i], width + 1, line, width);
		collisionTracker[i][width] = '\0';
		line = strtok_s(NULL, "\n", &nextLine);
	}

	return(collisionTracker);
}//end of getCollisionTracker
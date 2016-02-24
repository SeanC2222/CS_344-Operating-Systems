#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define MAZE_SIZE 7

struct Maze{
   struct Room* mazeRoom;
};


struct Room{
   char* name;
   FILE* roomFile;
   int type;
   struct Room** connections;
};

void buildMazeRooms(struct Maze*);
void writeMazeFiles(struct Maze*, int);
void freeMazeRooms(struct Maze*);
void rebuildMazeRooms(struct Maze*, int);
void initRoomName(struct Room*);
void initRoomConnections(struct Room*);
void randomizeNames(struct Maze*);
void randomizeConnections(struct Maze*);
void randomizeType(struct Maze*);
void getRoomName(struct Room*);
void getRoomConnections(struct Maze*, struct Room*);
void getRoomType(struct Room*);
int _enoughConnections(int, int, int*);
void freeRoomData(struct Room*);
int playGame(struct Maze*);

int main(){

   srand(time(NULL));

   int processId = getpid();

   struct Maze myMaze;
   buildMazeRooms(&myMaze);
   writeMazeFiles(&myMaze, processId);
   freeMazeRooms(&myMaze);
   rebuildMazeRooms(&myMaze, processId);

   int victoryCondition = playGame(&myMaze);

   freeMazeRooms(&myMaze);
   return 0;
}

/* Function: buildMazeRooms(struct Maze*)
 * Description: Parent function to build random Maze in dynamic memory.
 * Paramters:	  struct Maze* myMaze - Unallocated pointer (i.e. points to garbage)
 * Pre-conditions: myMaze is not pointing to anything useful.
 * Post-Conditions: myMaze is malloc'ed to contain MAZE_SIZE rooms, each room is malloc'ed and initialized,
 *	 each room's name and connection pointers are malloc'ed and intialized, random names and connections
 *	 are generated and stored, and a random type is generated and stored for each room.
 */
void buildMazeRooms(struct Maze* myMaze){
   //Allocate MAZE_SIZE rooms for myMaze
   myMaze->mazeRoom = malloc(MAZE_SIZE * sizeof(struct Room));

   //Loop to allocate and initialize room data
   int i;
   for(i = 0; i < MAZE_SIZE; i++){
      initRoomName(&myMaze->mazeRoom[i]);
      initRoomConnections(&myMaze->mazeRoom[i]);
   }
   //Picks random names for each room and stores as room name
   randomizeNames(myMaze);
   //Generates random connections for each room
   randomizeConnections(myMaze);
   //Picks a random START_ROOM and END_ROOM
   randomizeType(myMaze);
   return;
}

/* Function: writeMazeFiles(struct Maze*, int)
 * Description: Prints a fully generated Maze object's information to temporary room files in a directory
 * Paramters:	  struct Maze* myMaze - Fully allocated and initialized pointer to a Maze
 * Pre-conditions: myMaze and sub structs are allocated and initialized to values to be written to a file.
 * Post-Conditions: A directory is generated "./mulholls.rooms.$$" and MAZE_SIZE room files are written out
 *	 to this new directory. Each room file is of the following format:
 *	    ROOM NAME: [room name]     //current room
 *	    CONNECTION 1: [room name]  //adjacent rooms to current room
 *	    CONNECTION 2: [room name]	  //between 3 and 6 connections
 *	    ...			       
 *	    CONNECTION 6: [room name]  
 *	    ROOM TYPE: [room type]     //types of room include: START_ROOM, MID_ROOM, END_ROOM
 *	 All items malloc'ed in this function are freed prior to return.
 */
void writeMazeFiles(struct Maze* myMaze, int processId){

   //Malloc space for a c-string that will be the directory path data
   char* newDir;
   newDir = malloc (25 * sizeof(char));

   //Store new directory path name in newDir
   sprintf(newDir, "./mulholls.rooms.%i", processId);
   
   //Make directory
   int dirFail = mkdir(newDir, S_IRWXU | S_IRWXG | S_IRWXO);
   if (dirFail){
      printf("\nCan't create necessary directory.\n");
      return;
   }

   //Malloc space for a c-string that will be room file path data
   char* roomName = malloc(30 * sizeof(char));
   sprintf(roomName, "%s/room1", newDir);

   //Write out each room name to each file
   int i;
   for (i = 0; i < MAZE_SIZE; i++){
      //Set room number in path name (i.e. path name = "./mulholls.rooms.$$/room_")
      roomName[strlen(newDir) + 5] = i + 49;
      //Open the file and set to write status, store pointer in mazeRoom[i]'s roomFile
      myMaze->mazeRoom[i].roomFile = fopen(roomName,"w");
      //Write out "ROOM NAME: [room name]" to file
      fprintf(myMaze->mazeRoom[i].roomFile, "ROOM NAME: %s\n", myMaze->mazeRoom[i].name);
   }

   //Write out each connection and room type to each file
   for (i = 0; i < MAZE_SIZE; i++){
      //Set room number in path name (i.e. path name = "./mulholls.rooms.$$/room_")
      roomName[strlen(newDir) + 5] = i + 49;
      //Loop through each connection and print "CONNECTION #: [room name]" to file
      int j, connectionNumber = 1;
      for(j = 0; j < MAZE_SIZE; j++){
	 //If connection[j] is not null write name to file
	 if (myMaze->mazeRoom[i].connections[j] != NULL){
	    fprintf(myMaze->mazeRoom[i].roomFile,
		    "CONNECTION %i: %s\n", connectionNumber, myMaze->mazeRoom[i].connections[j]->name);
	    connectionNumber++;
	 }
      }

      //Write out "ROOM TYPE: [room type]" to file
      fprintf(myMaze->mazeRoom[i].roomFile, "ROOM TYPE: ");
      //0 = start room
      if (myMaze->mazeRoom[i].type == 0){
	 fprintf(myMaze->mazeRoom[i].roomFile, "START_ROOM");
      //2 = end room
      } else if (myMaze->mazeRoom[i].type == 2){
	 fprintf(myMaze->mazeRoom[i].roomFile, "END_ROOM");
      //1 = mid room
      } else {
	 fprintf(myMaze->mazeRoom[i].roomFile, "MID_ROOM");
      }
   }

   //Free locally used dynamic memory
   free(newDir);
   free(roomName);

   return;
}

/* Function: freeMazeRooms(struct Maze*)
 * Description: Frees dynamically allocated memory from initialized Maze object.
 * Parameters:	  struct Maze* myMaze - Maze object to be freed
 * Pre-Conditions: Fully allocated (Maze, Rooms, Names, RoomFile, & Connections) Maze object
 * Post-Conditions: Each room's roomFile is closed, the name and & connections are freed, and finally
 *	 the pointer to the rooms is freed.
 */
void freeMazeRooms(struct Maze* myMaze){
   int i;
   for(i = 0; i < MAZE_SIZE; i++){
      //Close mazeRoom[i].roomFile
      fclose(myMaze->mazeRoom[i].roomFile);
      //Free name and connection memory
      freeRoomData(&myMaze->mazeRoom[i]);
   }
   //Free mazeRoom memory
   free(myMaze->mazeRoom);
   return;
}

/* Function: rebuildMazeRooms(struct Maze*, int)
 * Description: Malloc's memory for a Maze object again, reads in data from stored files, and builds initializes
 *	 object from files.
 * Parameters:	  struct Maze* myMaze - uninitialzed Maze pointer.
 *		  int processId - Process Id of current program run (used to find temp files)
 * Pre-Conditions: MAZE_SIZE room files have been generated in "./mulholls.rooms.&&", and myMaze is uninitialized.
 * Post-Conditions: myMaze malloc's MAZE_SIZE worth of rooms, reads in and stores their names, then reads in and 
 *	 stores their connections and types. A full Maze object is malloc'ed but not freed when this is called.
 *	 Non-maze specific malloc's are freed prior to return.
 *
 */
void rebuildMazeRooms(struct Maze* myMaze, int processId){

   //Allocate MAZE_SIZE rooms for storing for Maze object
   myMaze->mazeRoom = malloc(MAZE_SIZE * sizeof(struct Room));

   //Initialize name and connections   
   int i;
   for(i = 0; i < MAZE_SIZE; i++){
      initRoomName(&myMaze->mazeRoom[i]);
      initRoomConnections(&myMaze->mazeRoom[i]);
   }   

   //Malloc's memory for directory path data
   char* dir;
   dir = malloc (25 * sizeof(char));

   //Initializes dir to contain directory path data
   sprintf(dir, "./mulholls.rooms.%i", processId);

   //Malloc's memory for room path information
   char* roomName = malloc(30 * sizeof(char));
   //Initializes roomName to current room path data
   sprintf(roomName, "%s/room1", dir);

   //Opens each room's roomFile and sets to read only
   for (i = 0; i < MAZE_SIZE; i++){
      roomName[strlen(dir) + 5] = i + 49;
      myMaze->mazeRoom[i].roomFile = fopen(roomName,"r");
   }

   //Sets each room's name
   for(i = 0; i < MAZE_SIZE; i++){
      getRoomName(&myMaze->mazeRoom[i]);
   }

   //Sets each room's connections and room types
      //NOTE: mazeRoom[i].connections is an array of room pointers. Names must be populated in object
      //to read name from file, search for which room has a matching name, and then point to that room
   for(i = 0; i < MAZE_SIZE; i++){
      getRoomConnections(myMaze, &myMaze->mazeRoom[i]);
      getRoomType(&myMaze->mazeRoom[i]);
   }

   //Free locally allocated memory
   free(dir);
   free(roomName);

   return;
}

/* Function: initRoomName(struct Room*)
 * Description: Malloc's cur->name to a 30 character c-string
 * Parameters:	  struct Room* cur - unallocated c-string pointer
 * Pre-Conditions: cur->name should be uninitialized
 * Post-Conditions:  cur->name is malloc'ed to a 30 character c-string
 */
void initRoomName(struct Room* cur){
   cur->name = malloc(30 * sizeof(char));
   return;
}

/* Function: initRoomConnections(struct Room*)
 * Description: Malloc's and initializes cur->connections. Malloc's MAZE_SIZE worth of connections.
 * Parameters:	  struct Room* cur - unallocated array of struct Room*
 * Pre-Conditions: cur->connections should be uninitialized 
 * Post-Conditions: cur->connections is malloc'ed and initialized to NULL
 */
void initRoomConnections(struct Room* cur){
   cur->connections = malloc(MAZE_SIZE * sizeof(struct Room*));
   int i;
   for(i = 0; i < MAZE_SIZE; i++){
      cur->connections[i] = NULL;
   } 
   return;
}

/* Function: randomizeNames(struct Maze*)
* Description: randomly chooses a name for each room in Maze object
* Parameters:	  struct Maze* myMaze - object to populate with random names
* Pre-Conditions: myMaze->mazeRoom[i].name must be malloc'ed
* Post-Conditions: Random names are assigned to all rooms in Maze object. Locally malloc'ed objects
*     are freed prior to return.
*/
void randomizeNames(struct Maze* myMaze){
   int random, count;
   int tracker[10];
   char** names;

   //Creates an array of 10 c-strings   
   names = malloc(10 * sizeof(char*));
   int i;
   for(i = 0; i < 10; i++){
      names[i] = malloc(15 * sizeof(char));
   }
   
   //Initializes each of the c-strings
   strcpy(names[0],"Scary Room\0");
   strcpy(names[1], "Bright Room\0");
   strcpy(names[2], "Dark Room\0");
   strcpy(names[3], "River Room\0");
   strcpy(names[4], "Ancient Room\0");
   strcpy(names[5], "Big Room\0");
   strcpy(names[6], "Hazy Room\0");
   strcpy(names[7], "Eerie Room\0");
   strcpy(names[8], "Safe Room\0");
   strcpy(names[9], "Small Room\0");
   
   //Initializes tracking array to 0 (FALSE)
   for(i = 0; i < 10; i++){
      tracker[i] = 0;
   }
   
   //Picks a random name, checks if its been used, assigns it to myMaze->mazeRoom[i].name
   count = 0;  //Keeps track of which room is being assigned a name
   while (count < MAZE_SIZE){
      //Pick a number between 0 and 9
      random = rand() % 10;
      
      //If that name has already been assigned, pick again
      if (tracker[random] != 0){
	 continue;
      }
      //Else copy the name into the Maze object
      strcpy(myMaze->mazeRoom[count].name, names[random]);
      //Set that name to taken, increment to next room
      tracker[random] = 1;
      count++;
   }

   //Free locally malloc'ed memory
   for(i = 0; i < 10; i++){   
      free(names[i]);
   }
   free(names);

   return; 
}

/* Function: randomizeConnections(struct Maze*)
* Description: Picks random connections, and points mazeRoom[i].connections[j] to point to that room
* Parameters:	  struct Maze* myMaze - Fully initialized Maze object
* Pre-Conditions: myMaze is fully initialized
* Post-Conditions: Between 3 and 6 connections are generated for each mazeRoom[i]. 
*/
void randomizeConnections(struct Maze* myMaze){

   int i, j;
   
   //Malloc and initilize connection counter to 0
   int* counter = malloc(MAZE_SIZE * sizeof(int));
   for(i = 0; i < MAZE_SIZE; i++){
      counter[i] = 0;
   }

   //Malloc and initialize a truth table to 0 (1 = connected, 0 = not connected)
   int** truthTable = malloc(MAZE_SIZE * sizeof(int*));
   for(i = 0; i < MAZE_SIZE; i++){
      truthTable[i] = malloc(MAZE_SIZE * sizeof(int));
      for(j = 0; j < MAZE_SIZE; j++){
	 truthTable[i][j] = 0;
      }
   }

   //Select a goal number of connections
   int numConnections = rand() % 10 + 11;

   //Sets truthTable diagonal to -1 (rooms can't connect to themselves)
   for(i = 0; i < MAZE_SIZE; i++){
      truthTable[i][i] = -1;
   }

   //While number of created connections(i) is less than numConnections AND there are less than 3 connections
      //per room, generate a random number in a and b, check if the connection should be made, and make the
      //connection.
   int a = 0, b = 0;
   i = 0;
   while(!_enoughConnections(i, numConnections, counter)){
      //Generate Random numbers in a and b
      a = rand() % MAZE_SIZE;
      b = rand() % MAZE_SIZE;
      //Reasoning:
	 //a != b - Rooms can't connect to themselves
	 //counter[a] < 6 - Rooms can't have more than 6 connections
	 //counter[b] < 6 - Rooms can't have more than 6 connections
	 //truthTable[a][b] != 1 - Don't increment counter if connection is already made
      if (a != b && counter[a] < 6 && counter[b] < 6 && truthTable[a][b] != 1){
	 //Set truth table for connection
	 truthTable[a][b] = 1;
	 truthTable[b][a] = 1;
	 //Increment counter for each room
	 counter[a]++;
	 counter[b]++;
	 //Increment total number of connections
	 i++;
      }
   }

   //Set connection pointer to point to rooms indicated by truth table
   for(i = 0; i < MAZE_SIZE; i++){
      for(j = 0; j < MAZE_SIZE; j++){
	 if (truthTable[i][j] == 1){
	    myMaze->mazeRoom[i].connections[j] = &myMaze->mazeRoom[j];
	 }
      }
   }

   //Free locally allocated memory
   for(i = 0; i < MAZE_SIZE; i++){
      free(truthTable[i]);
   }
   free(truthTable);
   free(counter);
   return;
}

/* Function: randomizeType(struct Maze*)
* Description: Initializes all mazeRoom[i].type to MID_ROOM(1), randomly pick a START_ROOM(0) and END_ROOM(2)
* Parameters:	  struct Maze* myMaze - Maze to have room types randomized
* Pre-Conditions: Fully initialized Maze object
* Post-Conditions: All mazeRoom[i].type's are initialized with random START_ROOM and END_ROOM
*/
void randomizeType(struct Maze* myMaze){
   //Array to track which room is START_ROOM and which is END_ROOM   
   int typeGenerator[MAZE_SIZE];

   //Initializes tracking array to all MID_ROOM(1)
   int i, random, finish = 0;
   for(i = 0; i < MAZE_SIZE; i++){
      typeGenerator[i] = 1;
   }

   //Generate a random start location and set to START_ROOM(0)
   random = rand() % MAZE_SIZE;
   typeGenerator[random] = 0;
   
   //Pick a second location until it's not the same as START_ROOM, and set as END_ROOM(2)
   while(!finish){
      random = rand() % MAZE_SIZE;
      if(typeGenerator[random] == 0){
	 continue;
      }
      typeGenerator[random] = 2;
      finish = 1;
   }

   //Sets each mazeRoom[i].type to the same value as typeGenerator[i]
   for(i = 0; i < MAZE_SIZE; i++){
      myMaze->mazeRoom[i].type = typeGenerator[i];
   }
   return;
}

/* Function: getRoomName(struct Room*)
* Description: Find and get room name in current room pointed to by cur->roomFile, and store as cur->name
* Parameters:	  struct Room* cur - current room to check roomFile and set name for
* Pre-Conditions: Room file for cur is already generated. cur->roomFile points to first line of open room file.
* Post-Conditions: cur->name is set to cur->roomFile's room name 
*/
void getRoomName(struct Room* cur){

   //Progress file pointer until char after ':'
   while(fgetc(cur->roomFile) != ':');

   //Progress file pointer 1 char
   fgetc(cur->roomFile);

   //Get the rest of the line and store in cur->name
   char* temp = NULL;
   size_t n = 0; //necessary for getline()
   getline(&temp, &n, cur->roomFile);
   strcpy(cur->name, temp);

   //Remove '\n' from end of string
   cur->name[strlen(cur->name)-1] = '\0';
   free(temp);
   return;
}

/* Function: getRoomConnections(struct Maze*, struct Room*)
* Description: Find and set room connections in Maze object from cur->roomFile
* Parameters:	  struct Maze* myMaze - The Maze object being populated from files, mazeRoom[i].name's must be 
*		     populated
*		  struct Room* cur - current room for which connections are being made
* Pre-Conditions: myMaze->mazeRoom[i].name must be initialized, cur->roomFile must be open and readable,
*     cur->connections[i] must be initialized, cur->roomFile should be pointing at first "CONNECTION #:" line.
* Post-Conditions: cur->connections[i] will point to room connections. Locally allocated memory is freed.
*/
void getRoomConnections(struct Maze* myMaze, struct Room* cur){
   char* temp = NULL;
   size_t n = 0;
   int numConnect = 0;

   //While first character of current line is 'C'
   while(fgetc(cur->roomFile) == 'C'){
      //Progress file pointer to char after ':'
      while(fgetc(cur->roomFile) != ':');
      //Progress file pointer 1 char
      fgetc(cur->roomFile);
      //Store the rest of line in temp
      getline(&temp, &n, cur->roomFile);
      //Remove '\n' from end of string
      temp[strlen(temp)-1] = '\0';

      //Search myMaze->mazeRoom[i] for name that matches temp
      int i = 0; //i will store index of myMaze->mazeRoom[i] where temp == mazeRoom[i].name
	 //Note: COULD CAUSE SEG_FAULT IF PRE-CONDITIONS AREN'T MET
      while(strcmp(temp, myMaze->mazeRoom[i].name) != 0){
	 i++;
      }

      //Points connections[numConnect] to mazeRoom[i] with matching name
      cur->connections[numConnect] = &myMaze->mazeRoom[i];
      numConnect++;      
   }
   
   //Free locally allocated memory
   free (temp);
   return;
}

/* Function: getRoomType(struct Room*)
* Description: Find and set room type in cur
* Parameters:	  struct Room* cur - current room that is getting a type
* Pre-Conditions: cur->roomFile is open, readable, and the file pointer should be at the beginning of the 
*     "ROOM TYPE: " line.
* Post-Conditions: cur->type is set to 0 if the room file says "START_ROOM", 2 if the room says "END_ROOM" or
*     else 1. Locally allocated memory is freed.
*/
void getRoomType(struct Room* cur){
   char* temp = NULL;
   size_t n = 0;

   while(fgetc(cur->roomFile) != ':');
   fgetc(cur->roomFile);
   getline(&temp, &n, cur->roomFile);

   if (strcmp(temp, "START_ROOM") == 0){
      cur->type = 0;
   } else if (strcmp(temp, "END_ROOM") == 0){
      cur->type = 2;
   } else {
      cur->type = 1;
   }

   free(temp);
   return;
}

/* Function: _enoughConnections(int, int, int*)
* Description: Child boolean function to check if Maze object needs more connections, and that each room has at
*     least 3 connections. Used ONLY in randomizeConnections()
* Parameters:	  int i - current number of assigned connections by program. CAN be greater than n if there is
*			  a room that doesn't have a full 3 connections yet.
*		  int n - randomly generated goal number of total connections. If each room has 3 connections
*			  and goal is met, function returns true. If each room has 3 connections but goal isn't
*			  met yet, function returns false.
*		  int* counter - pointer to array of connection counts for each room.
* Pre-Conditions: counter is initialized.
* Post-Conditions: No values are changed, if enough connections exist for the game to run return true, else
*     return false.
*/
int _enoughConnections(int i, int n, int* counter){
   int j;
   for(j = 0; j < MAZE_SIZE; j++){
      if (i < n || counter[j] < 3){
	 return 0;
      }
   }

   return 1;
}

/* Function: freeRoomData(struct Room*)
* Description: Frees a room's dynamically allocated data.
* Parameters:	  struct Room* cur - current room to free dynamically allocated data
* Pre-Conditions: cur->name and cur->connections must be dynamically allocated.
* Post-Conditions: cur->name and cur->connections' memory is freed.
*/
void freeRoomData(struct Room* cur){
   free(cur->name);
   free(cur->connections);
   return;
}

/* Function: playGame(struct Maze*)
* Description: Primary gameplay function. Displays current location, connections, and prompts user for action.
* Parameters:	  struct Maze* myMaze - Fully initialzed Maze object with accurate connections in place.
* Pre-Conditions: myMaze must be fully initialized.
* Post-Conditions: function returns 1 if END_ROOM is reached in 100 actions, or returns 0 if END_ROOM is not
*     reached within 100 user actions.
*/
int playGame(struct Maze* myMaze){
   
   //Find Start room
   int i = 0;
   struct Room* curRoom;
   while(myMaze->mazeRoom[i].type){
      i++;
   }
   curRoom = &myMaze->mazeRoom[i];
   
   char* userInput = malloc(100 * sizeof(char));

   int movementCount = 0;
   char** moveList = malloc(100 * sizeof(char*));
   while(curRoom->type != 2){
      printf("\nCURRENT LOCATION: %s\n", curRoom->name);
      printf("POSSIBLE CONNECTIONS:");

      i = 0;
      while(curRoom->connections[i] != NULL){
	 printf(" %s", curRoom->connections[i]->name);
	 if(curRoom->connections[i+1] == NULL){
	    printf(".\n");
	 } else {
	    printf(",");
	 }
	 i++;
      }
      //Prompt user for input
      printf("WHERE TO?>");
      //Get input from stdin
      fgets(userInput, 99, stdin);
      //Remove extra '\n' at end
      userInput[strlen(userInput)-1] = '\0';

      //Check curRoom->connections[i] to see if the room is connected, if found curRoom is update to new room
      for(i = 0; i < MAZE_SIZE; i++){
	 if(curRoom->connections[i] != NULL && strcmp(userInput, curRoom->connections[i]->name) == 0){
	    curRoom = curRoom->connections[i];
	    moveList[movementCount] = curRoom->name;
	    movementCount++;
	    break;
	 }
      }

      //If all the rooms are searched, and input doesn't have a match tell the user.
      if(i == MAZE_SIZE){
	 printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
      }

      //If player taks 100 moves, he/she dies and game ends.
      if(movementCount >=100){
	 printf("\nAFTER SEARCHING TIRELESSLY YOUR BODY GIVES OUT. YOU GASP FOR BREATH\n");
	 printf("KNOWING THAT YOU HAVE FAILED IN YOUR ESCAPE. YOUR EYES BEGIN TO CLOSE\n");
	 printf("AND WITH YOUR DYING BREATHS YOUR REALIZE THE %s IS THE LAST\n", curRoom->name);
	 printf("PLACE YOU'LL EVER SEE.\n");
	 return 0;
      }
   }

   printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
   printf("YOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n", movementCount);
   for(i = 0; i < movementCount; i++){
      printf("%s\n", moveList[i]);
   }

   free(moveList);
   free(userInput);
   return 1;
}


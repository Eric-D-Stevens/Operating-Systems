// Due to the juggling of learning the material
// and keeping the context of the assignment,
// this code is very convoluted. That being 
// said all goals of the assignment should be
// met. File reading is done in manyplaces and
// in many functions and the best way to find
// those places is by searching "fgets".

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

// This structure is used to set up game map and output the files.
struct room_info {
	char* name; // name of room
	char* type; // START MID or END room
	int numConnections; // How many connections room has
	char* connections[6]; // List of connections names

};


int CreateDirectory(); // Creates directory
int GenerateMap(); // Generates game map layout
int GenerateRoomFile(struct room_info *theRoom, char* filename); // Creates the files with the room information
int FindStart(char* theStart); // Searches through files to find START_ROOM string and set as starting place
int PosCon(int room); // Reads current room file and outputs possible connections from that room
int GoToRoom(char* entry); // Searches through all files to find entered string and makes that the current room
int IsConnections(char* entry, int curRoom); // Checks to see if user input is infact an availible connectiopn
char* getName(int room); // Used to get the room name of the starting room
int GameOver(int steps, char* filename); // Declares a win and ouput number of steps and path taken
int FindEnd(int CurRoom); // Looks to see if the current room is the END ROOM
int IsaRoom(char* input); // Checks to see that what was entered is a room that exists






int main(void){
	
	// Create directory
	CreateDirectory();
	
	// Open temp file to store steps
	FILE *record;
	char* recordfile = "record";
	record = fopen(recordfile,"w"); //write to
	int numOfTurns = 0; //keeps track of number of steps
	
	system("clear"); // Clear the screen
	srand((int)time(NULL)); // seed rand() with time
	
	// Generate the map
	GenerateMap();	
	
	int startRoom; // file number of start room
	startRoom = FindStart("START_ROOM"); // find it

	char* startName; // room name of starting room
	startName = getName(startRoom); // set the name
	char hope[14];	
	sprintf(hope,"%s", startName); // copy to array
	GoToRoom(hope); // enter the starting room

	int inside = 0; // bool to check if there is a connection
	char input[20]; // used as command line input
	
	printf("WHERE TO? >");	// prompt user
	scanf("%s",input); // get input
// this step is done because the check for valid connections only needed part of the matching stirng
	if(IsaRoom(input)==0){strcat(input,"!!!!!!");} // if input matches no rooms alter stirng
	inside = IsConnections(input, startRoom); // is input a possible connection
	if(inside == 0){GoToRoom(hope); strcpy(input,hope);} // if input is a connection go to room
	
// This loops consistently until the game is won
while(1){
	//Loop while input is not a valid connection
	while(!inside){ 
		printf("WHERE TO? >"); // prompt user
		char temp[20]; // temp string to store current room
		sprintf(temp,"%s",input); // store current room
		scanf("%s",input); // get input
// this step is done because the check for valid connections only needed part of the matching stirng
		if(IsaRoom(input)==0){strcat(input,"!!!!!!");} // if input is not a valid room alter string
		inside = IsConnections(input, startRoom); // is input valid connection
		if(inside == 0){GoToRoom(temp); strcpy(input,temp);} // if input is valid connection go to room
	}

	numOfTurns++; // add to number of steps upon valid connection
	printf("\n\n");
	startRoom = GoToRoom(input); // go to valid room
	fprintf(record,input); // record valid room name to record 
	fprintf(record,"\n");

	if(FindEnd(startRoom)){ // if the room entered is the END ROOM
		fclose(record);	// close the record file
		GameOver(numOfTurns,"record"); // congratulate and print record
	}
	inside = 0; // reset to invalid input
}
	
	




return 0;
}







// This function will create the new directory //
// and will then change the working directory  //
// of the process to the created directory.    //
int CreateDirectory(){
	int PID;	
	char dir1[20];	
	PID = getpid();	// Get Process ID
	sprintf(dir1,"steveric.rooms.%d",PID);	// Create Directory w/ PID
	mkdir(dir1, ACCESSPERMS);	// Make directory
	chdir(dir1);	// Change process working directory
		

return 0;
}

// This function will generate the map by. It //
// is a hardcoded map but the rooms and thier //
// positions are randomized.                  //

int GenerateMap(){
	
	// list of possible rooms
	char* room_name[10];
	room_name[0] = "yard";
	room_name[1] = "pool";
	room_name[2] = "street";
	room_name[3] = "ally";
	room_name[4] = "garage";
	room_name[5] = "closet";
	room_name[6] = "attic";
	room_name[7] = "kitchen";
	room_name[8] = "basement";
	room_name[9] = "toilet";
		
	
	int normalizer[10];// normal 0 - 9 array
	int randomizer[10];// used to generate random array order
	int j;
	int randLoc;
	char* rando_room[10]; // array to be filled with rooms in random order

	for(j=0; j<10; j++){normalizer[j] = j;} // create 0 - 9 array

	// shuffel numbers 0-9 into random array
	for(j=0; j<10; j++){
		do{randLoc = rand()%10;}while(normalizer[randLoc]==-1);
		randomizer[j] = normalizer[randLoc];
		normalizer[randLoc] = -1;
	}
	
	for(j=0; j<10; j++){rando_room[j] = room_name[randomizer[j]];} // fill random name array

	
////////////////////////////
/////////// MAP 1 //////////
////////////////////////////


// Room 0 - Start Room //
struct room_info room0;			// Declar room sturct
room0.name = rando_room[0];		// set room name
room0.type = "START_ROOM";		// set room type
room0.numConnections = 4;		// set number of connections
room0.connections[0] = rando_room[1]; 	// set connection 1 to room 
room0.connections[1] = rando_room[3];	// set connection 2 to room 
room0.connections[2] = rando_room[4];	// set connection 3 to room 
room0.connections[3] = rando_room[5];	// set connection 4 to room 
char* filename0 = "room_0";		// set file name

// Room 1 //
struct room_info room1;			// see above
room1.name = rando_room[1];
room1.type = "MID_ROOM";
room1.numConnections = 4;
room1.connections[0] = rando_room[0]; 
room1.connections[1] = rando_room[2];
room1.connections[2] = rando_room[3];
room1.connections[3] = rando_room[6];
char* filename1 = "room_1";

// Room 2 //
struct room_info room2;			// see above
room2.name = rando_room[2];
room2.type = "MID_ROOM";
room2.numConnections = 4;
room2.connections[0] = rando_room[1]; 
room2.connections[1] = rando_room[4];
room2.connections[2] = rando_room[5];
room2.connections[3] = rando_room[6];
char* filename2 = "room_2";

// Room 3 //
struct room_info room3;			// see above
room3.name = rando_room[3];
room3.numConnections = 3;
room3.connections[0] = rando_room[0]; 
room3.connections[1] = rando_room[1];
room3.connections[2] = rando_room[4];
char* filename3 = "room_3";

// Room 4 //
struct room_info room4;			// see above
room4.name = rando_room[4];
room4.type = "MID_ROOM";
room4.numConnections = 4;
room4.connections[0] = rando_room[0]; 
room4.connections[1] = rando_room[2];
room4.connections[2] = rando_room[3];
room4.connections[3] = rando_room[5];
char* filename4 = "room_4";

// Room 5 //
struct room_info room5;			// see above
room5.name = rando_room[5];
room5.type = "MID_ROOM";
room5.numConnections = 4;
room5.connections[0] = rando_room[0]; 
room5.connections[1] = rando_room[2];
room5.connections[2] = rando_room[4];
room5.connections[3] = rando_room[6];
char* filename5 = "room_5";

// Room 6 //
struct room_info room6;			// see above
room6.name = rando_room[6];
room6.type = "END_ROOM";
room6.numConnections = 3;
room6.connections[0] = rando_room[1]; 
room6.connections[1] = rando_room[2];
room6.connections[2] = rando_room[5];
char* filename6 = "room_6";


// Generate the room files based off each room struct instance
GenerateRoomFile(&room0, filename0);
GenerateRoomFile(&room1, filename1);
GenerateRoomFile(&room2, filename2);
GenerateRoomFile(&room3, filename3);
GenerateRoomFile(&room4, filename4);
GenerateRoomFile(&room5, filename5);
GenerateRoomFile(&room6, filename6);


	return 0;
}





// This function is responsible for taking in a room  //
// struct and generating a text file as specified in  //
// assignment specification.			      //
int GenerateRoomFile(struct room_info *theRoom, char* filename){
	
	// Open the file
	FILE * fp;
	fp=fopen(filename, "w");

	fprintf(fp, "ROOM NAME: %s\n", theRoom->name); // output room name to file

	int i; 
	for(i=0; i<theRoom->numConnections; i++){ // for i is less than number of connecting room
		fprintf(fp, "CONNECTION %d: %s\n",i+1,theRoom->connections[i]); // out put connection i to file
	}				
	fprintf(fp,"ROOM TYPE: %s\n",theRoom->type); // output room type to file
	fclose(fp);	// close the file for writing
	return 0;
}


// This function will find which of the generated    //
// files includes the string "START_ROOM" and 	     //
// the number of that file.			     //
int FindStart(char* theStart){
	
	int r;
	for(r=0; r<7; r++){ // for r < the number of files generated
		char fName[10]; // file name string
		sprintf(fName, "room_%d", r); // create file name ending in r
		FILE *fid;  
		fid = fopen(fName, "r");	 // open file
		
		char buff[100]; // buffer
		while(fgets(buff, 100, fid) != NULL){ // while not at end of file
			if((strstr(buff,theStart)) != NULL){return r;}  // return file number when string is found
			}
		fclose(fid);
	}
	return 0;
}


// This function looks at the file of the room number //
// that is handed as a parameter and checks to see if //
// it contains the string "END_ROOM".		      //
int FindEnd(int CurRoom){
		
	char fName[10]; //file name string
	sprintf(fName, "room_%d", CurRoom); // set filename to input room
	FILE *fid;
	fid = fopen(fName, "r"); // open file for reading
		
	char buff[100]; // buffer
	while(fgets(buff, 100, fid) != NULL){ // while not at end of file
		if((strstr(buff,"END_ROOM")) != NULL){return 1;} // if "END_ROOM" found retrun 1
	}
	fclose(fid);
	
	return 0;
}


// This function takes a room number as an input and //
// goes through that file number to find connection  //
// strings. The connection are ouptu to stdout       //
int PosCon(int room){

	FILE *fp;
	char filename[10];
	sprintf(filename, "room_%d", room); // filename set
	char* con = "CONNECTION "; // search string set
	fp = fopen(filename, "r"); // open file
	char searchBuf[15];  // buffer
	char output[200]; // output string
	sprintf(output, "POSSIBLE CONNECTIONS: "); // output preamble

	while(fgets(searchBuf,15,fp) != NULL){ // while not at end of file
		if((strstr(searchBuf, con)) != NULL){ // if the "CONNECTION" is found
		fgets(searchBuf,15,fp); // Get the next 15 characters int the buffer
		strcat(output, searchBuf); // concatinate onto output string
		strcat(output, " "); // add a space between words	
		}
	}

	
	fclose(fp);
	int i;
	char a;
	char b;
	// this loop will replace the final newline character a null character
	for(i=0; i<200; i++){ // while i < than size of output string
		a = output[i]; 
		b = '\n';
		if(a == b){output[i]=',';}
		if(a == '\0' ){output[i-2] = '\n';break;}
	}

	printf("%s",output);	// output to stdout	
	
	return 0;
}


// This fuction is responsible for the output of the current location //
// It takes an input string and searches through all the files until  //
// if finds the one with the name handed to it and retruns that number//
int GoToRoom(char* entry){
	
	// Set entry file name	
	char toFind[30];
	sprintf(toFind,"ROOM NAME: %s",entry);		
	int roomNum;
	// Return file number that matches entry
	roomNum = FindStart(toFind); // returns file number of any string handed to it	

	char filename[20];
	FILE *fp;	
	sprintf(filename, "room_%d",roomNum); // create filename string

	fp = fopen(filename, "r"); // open the file for reading
		
		printf("CURRENT LOCATION: %s\n", entry); // output current location string
		PosCon(roomNum);		// output connection
	fclose(fp);	


return roomNum;
}

// This function looks through the current froom and checks to see //
// if the string handed to the function is a connection that could //
// be moved to.							   //
int IsConnections(char* entry, int curRoom){

	char filename[10];
	sprintf(filename,"room_%d",curRoom); // create file name for current room
	char connection[30]; // connection string for comparison
	int i;
	

	for(i=0; i<7; i++){	// while i is less than the number of possible connections
	sprintf(connection,"CONNECTION %d: %s",i,entry); // copy entry to string
	FILE *fp;
	char buff[50];
	fp = fopen(filename,"r"); // open file for reading
	while(fgets(buff,50,fp) != NULL){ // while not at end of file
		if((strstr(buff, connection)) != NULL){ // if the connection enteed matches
			return 1; // return 1
		}
	}
	fclose(fp);
	}
	// else
	printf("\n\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n\n"); // error message


	return 0; // and return 0
}


// This function is responsible for getting the name of the //
// starting room.					    //
char* getName(int room){
	FILE *fp;
	char filename[10]; 
	sprintf(filename, "room_%d", room); // generate file name of handed room number
	char* con = "ROOM NAME: "; // comparison string
	fp = fopen(filename, "r"); // open file
	char searchBuf[20];  // buffer
	char output[200]; // hold name once found
	while(fgets(searchBuf,12,fp) != NULL){ // while not at end of file
		if((strstr(searchBuf, con)) != NULL){ //if name string is found
		fgets(searchBuf,20,fp); // pull next 20 chars
		strcpy(output, searchBuf); // copy to output
		}
	}
	char *pos; // pointer for deleting unwanted char
	if((pos=strchr(output,'\n')) != NULL);{*pos = '\0';}//delete unwanted char
	char* roomName=output; // set ouptu to return value
	return roomName; // return room name
}


// Simple function that lets the the user know that the have //
// made it to the end room and prints the number of steps    //
// taken and the contents of the record file 		     //
int GameOver(int steps, char* filename){

	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:", steps);

	FILE *fp;
	fp = fopen(filename,"r");	// open record file
	char buff[500]; // buffer
	printf("\n"); 
	while(fgets(buff, 500, fp) != NULL){printf("%s",buff);} // while not at EOF output contents
	fclose(fp);
	printf("\n\n\n\n\n");
	exit(0);
return 0;
}

// This function checks to see if the string that it is     //
// passed matches any of the possible room names.   	    //
int IsaRoom(char* input){
	// list of possible room names
	char* room_name[10];
	room_name[0] = "yard";
	room_name[1] = "pool";
	room_name[2] = "street";
	room_name[3] = "ally";
	room_name[4] = "garage";
	room_name[5] = "closet";
	room_name[6] = "attic";
	room_name[7] = "kitchen";
	room_name[8] = "basement";
	room_name[9] = "toilet";
	
	int i;
	// check aginst all room names
	for(i=0; i<10; i++){
		if(strstr(input, room_name[i])){return 1;} // if match
	}
	return 0; // no match
}
	





#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

struct room_info {
	char* name;
	char* type;
	int numConnections;
	char* connections[6];

};


int CreateDirectory();
//int CreateFiles();
int GenerateMap();
int GenerateRoomFile(struct room_info *theRoom, char* filename);

int main(void){

	srand((int)time(NULL));
	CreateDirectory();
	GenerateMap();	



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

int GenerateMap(){

	char* room_name[10];
	room_name[0] = "Bed_Room";
	room_name[1] = "Living_Room";
	room_name[2] = "Bath_Room";
	room_name[3] = "Game_Room";
	room_name[4] = "Garage";
	room_name[5] = "Closet";
	room_name[6] = "Attic";
	room_name[7] = "Kitchen";
	room_name[8] = "Dining_Room";
	room_name[9] = "Laundry_Room";
		
	
	int normalizer[10];
	int randomizer[10];
	int j;
	int randLoc;
	char* rando_room[10];

	for(j=0; j<10; j++){
		normalizer[j] = j;
		printf("%d\n",j);	
	}

	
	for(j=0; j<10; j++){
		do{randLoc = rand()%10;}while(normalizer[randLoc]==-1);
		randomizer[j] = normalizer[randLoc];
		normalizer[randLoc] = -1;
		printf("\n%d",randomizer[j]);
	}
	
	for(j=0; j<10; j++){
		rando_room[j] = room_name[randomizer[j]];

	}


	int numOfStr;
	numOfStr = sizeof(room_name);
	printf("\n\n %d \n\n",numOfStr);
	
	int i = 0;
	for(i=0;i<10;i++){
	printf("%s\n",rando_room[i]);
	}
////////////////////////////
/////////// MAP 1 //////////
////////////////////////////


// Room 0 - Start Room //
struct room_info room0;
room0.name = rando_room[0];
room0.type = "START_ROOM";
room0.numConnections = 4;
room0.connections[0] = rando_room[1]; 
room0.connections[1] = rando_room[3];
room0.connections[2] = rando_room[4];
room0.connections[3] = rando_room[5];
char* filename0 = "room_0";

// Room 1 //
struct room_info room1;
room1.name = rando_room[1];
room1.type = "MID_ROOM";
room1.numConnections = 4;
room1.connections[0] = rando_room[0]; 
room1.connections[1] = rando_room[2];
room1.connections[2] = rando_room[3];
room1.connections[3] = rando_room[6];
char* filename1 = "room_1";

// Room 2 //
struct room_info room2;
room2.name = rando_room[2];
room2.type = "MID_ROOM";
room2.numConnections = 4;
room2.connections[0] = rando_room[1]; 
room2.connections[1] = rando_room[4];
room2.connections[2] = rando_room[5];
room2.connections[3] = rando_room[6];
char* filename2 = "room_2";

// Room 3 //
struct room_info room3;
room3.name = rando_room[3];
room3.type = "MID_ROOM";
room3.numConnections = 3;
room3.connections[0] = rando_room[0]; 
room3.connections[1] = rando_room[1];
room3.connections[2] = rando_room[4];
char* filename3 = "room_3";

// Room 4 //
struct room_info room4;
room4.name = rando_room[4];
room4.type = "MID_ROOM";
room4.numConnections = 4;
room4.connections[0] = rando_room[0]; 
room4.connections[1] = rando_room[2];
room4.connections[2] = rando_room[3];
room4.connections[3] = rando_room[5];
char* filename4 = "room_4";

// Room 5 //
struct room_info room5;
room5.name = rando_room[5];
room5.type = "MID_ROOM";
room5.numConnections = 4;
room5.connections[0] = rando_room[0]; 
room5.connections[1] = rando_room[2];
room5.connections[2] = rando_room[4];
room5.connections[3] = rando_room[6];
char* filename5 = "room_5";

// Room 6 //
struct room_info room6;
room6.name = rando_room[6];
room6.type = "END_ROOM";
room6.numConnections = 3;
room6.connections[0] = rando_room[1]; 
room6.connections[1] = rando_room[2];
room6.connections[2] = rando_room[5];
char* filename6 = "room_6";


GenerateRoomFile(&room0, filename0);
GenerateRoomFile(&room1, filename1);
GenerateRoomFile(&room2, filename2);
GenerateRoomFile(&room3, filename3);
GenerateRoomFile(&room4, filename4);
GenerateRoomFile(&room5, filename5);
GenerateRoomFile(&room6, filename6);


	return 0;
}



//////////////////////////////////////////////////




int GenerateRoomFile(struct room_info *theRoom, char* filename){
	
	printf("\nmade it into gen function");
	FILE * fp;
	fp=fopen(filename, "w");
	fprintf(fp, "ROOM NAME: %s\n", theRoom->name);

	int i; 
	for(i=0; i<theRoom->numConnections; i++){
		fprintf(fp, "CONNECTION %d: %s\n",i+1,theRoom->connections[i]);
	}				
		fprintf(fp,"ROOM TYPE: %s\n",theRoom->type);
	
	

	fclose(fp);	




	return 0;
}

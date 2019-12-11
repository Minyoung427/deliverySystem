#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *contents : package context (message string)
*/
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;

static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password



// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {
	
	storage_t storage = {0,0,0};	//initialize storage's information as initial values
	deliverySystem[x][y] = storage;		//input storage's information in deliverySystem
	
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	
	char passwd[PASSWD_LEN+1];
	
	printf(" input password for (%i, %i) storage : ",x,y);
	scanf("%4s", &passwd);		// input password 
	
	fflush(stdin); 
	
	if(strcmp(deliverySystem[x][y].passwd,passwd) == 0)
		return 0;		// when saved password and input password are same, return 0
	else 
		return -1;		// when saved password and input password are different, return -1
	
}






// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	
	FILE *f_p = fopen(filepath, "w");  //open file as write mode
	
	int a, b;
   
   	fprintf(f_p, "%i %i\n", systemSize[0], systemSize[1]);            //print row/column of the delivery system at file
   	fprintf(f_p, "%s\n", masterPassword);                        //print master password at file

   	for(a=0; a<systemSize[0]; a++){                   //repeat print deliverySystem's information (building, room, password and context) at file  	 
      for(b=0; b<systemSize[1]; b++){
         if (deliverySystem[a][b].cnt>0){                   // when package exist, print deliverySystem's information
            fprintf(f_p, "%d %d %d %d %s %s\n", a, b, deliverySystem[a][b].building, deliverySystem[a][b].room, deliverySystem[a][b].passwd, deliverySystem[a][b].context);
         }
      }
   	}
	fclose(f_p);		//close pf file

	return 0;
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	
	int a, b;	

	FILE *f_p = fopen(filepath,"r");		//open the file as read mode
	
		
	if(f_p!= NULL)		//if file opened
	{

		fscanf(f_p, "%i %i", &systemSize[0], &systemSize[1]);		//scan row, column from file f_p
		fscanf(f_p, "%s", &masterPassword);		// masterpassword from file f_p

		deliverySystem = (storage_t **) malloc(systemSize[0] *sizeof(storage_t *));		//assign row value
		
		for (a=0;a<systemSize[0];a++){		//initialize storage 
			deliverySystem[a] = (storage_t *) malloc(systemSize[1] * sizeof(storage_t));		//assign column value
				for (b=0;b<systemSize[1];b++) 
				{
					initStorage(a,b);
				}
		}

		while(feof(f_p)==0){
			storage_t storage={.passwd={}, .context=(char*)malloc(sizeof(char) * (MAX_MSG_SIZE+1))};
			int a=-1, b=-1;		//reset a,b value as -1
			
			fscanf(f_p, "%d %d %d %d %s %s", &a, &b, &storage.building, &storage.room, &storage.passwd, storage.context);
			if (a>=0 && b>=0){
				storage.cnt = strlen(storage.context);
				deliverySystem[a][b]=storage;
				
				storedCnt++;
			}
		}
		fclose(f_p);		//close file f_p
	} 
	else 
		return -1;
	
	return 0;
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	
	int i;
	
	for(i=0; i<systemSize[0]; i++){		//free the memory all of the deliverysystem by repeat
		free(deliverySystem[i]);
	}
	
	free(deliverySystem);
}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {
	
	storage_t storage = {0, 0, 0};	
	initStorage(x, y);		//before push, initialize storage
	deliverySystem[x][y].building = nBuilding;	
	deliverySystem[x][y].room = nRoom;
	strcpy(deliverySystem[x][y].passwd, passwd);		//copy passwd to deliverysystem's passwd 	
	deliverySystem[x][y].context = msg;
	deliverySystem[x][y].cnt = sizeof(deliverySystem[x][y].context);		//make space for input package as size of context

	if(deliverySystem[x][y].cnt <= 0) 		// if cannot push, return -1
		return -1;
	
	storedCnt++;	
	
	return 0;
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
	if(inputPasswd(x,y) == 0) 		//if password is correct with saved password run this sentence
	{
		printStorageInside(x,y);		//printing delivery context
		initStorage(x,y);		 //initialize the storage
		storedCnt--;		 //minus 1 at storedCnt
	} 
	else 		//if the entered password different with originally information
		return -1;	

	return 0;
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	
	int a,b;
	int cnt = 0;		//initialize cnt value as 0 cnt value
	
	for(a=0; a<systemSize[0]; a++){
		for(b=0; b<systemSize[1]; b++){
			if(deliverySystem[a][b].building==nBuilding && deliverySystem[a][b].room==nRoom){
				printf(" -----------> Found a package in (%d, %d)\n", a, b);		// print row and column of that storage 
				
				cnt++;		// if find package, plus 1 at cnt
			}
		}
	}
	
	return cnt;
}

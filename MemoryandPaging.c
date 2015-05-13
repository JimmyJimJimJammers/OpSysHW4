//Matthew DiLandro
//Assignment 4
//Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <time.h>
#include <limits.h>

#define AllPageTablesSize 1024
#define numFrames 32
#define frameSize 1024
#define numPages 4

struct PageTable{
	char * Name;
	int * PageNumber;      //array of page number
	int * MemoryLocation;  //indices stored parallel to PageNumber
	int offset;
	clock_t PageTableUseTime;
};

struct Memory{ 
	char ** frames;
	struct PageTable ** AllPageTables;
	int freeFrames;
	int numPageTables;
};

struct PageTable * CreatePageTable(){
	struct PageTable * newPageTable = malloc(sizeof(struct PageTable));
	newPageTable->PageNumber = calloc(numPages, sizeof(int));
	int i;
	for(i=0;i<numPages;i++){
		newPageTable->PageNumber[i] = -1;
	}
	newPageTable->MemoryLocation = calloc(numPages, sizeof(int));
	for(i=0;i<numPages;i++){
		newPageTable->MemoryLocation[i] = -1;
	}
	newPageTable->offset = 0;
	newPageTable->PageTableUseTime = clock();                           //remind miguel to make clock();
	return newPageTable;
}

struct Memory * CreateMemory(){
	struct Memory *  MemoryBank = malloc(sizeof(struct Memory));
	MemoryBank->frames = calloc(numFrames, frameSize);
	MemoryBank->AllPageTables = malloc(AllPageTablesSize);
	MemoryBank->freeFrames = numFrames;
	MemoryBank->numPageTables = 0;
	return MemoryBank;
}

//~ int findMaxElement(struct Memory* MemoryBank, int* indices){ //finds max element in an array and returns that index
    //~ int maxValIndex = INT_MAX;
//~ }

void DeleteOldest(struct Memory* MemoryBank, int deletions){
    int removedframes;
    struct PageTable ** Temp = calloc(MemoryBank->numPageTables,sizeof(struct PageTable));
    int i;
    for (i = 0; i < MemoryBank->numPageTables; i++){
        Temp[i] = MemoryBank->AllPageTables[i];
    }
    //Sort the PageTables
    
    for(i=0;i<numPages;i++){
	if(Temp[0]->PageNumber[i] == -1){
		//end of page table
	}
	else{
		//Delete it from cache
		MemoryBank->frames[Temp[0]->MemoryLocation[i]] = "-1";
		Temp[0]->PageNumber[i] = -1;
		Temp[0]->MemoryLocation[i] = -1;
		removedframes = removedframes + 1;
	}
    }
    if(removedframes >= deletions){
		//return EXIT_SUCCESS;
    }
    //See if the removed frames where enough if not rerun the function
}

//~ int* findEmpty(struct Memory* MemoryBank, int framesNecessary){
    //~ if (framesNecessary > MemoryBank->numFiles){
        //~ framesNecessary = MemoryBank->numFiles;
    //~ }
    //~ if (framesNecessary > MemoryBank->freeFrames){
        //~ //delete that many files
        //~ DeleteOldest(MemoryBank, (framesNecessary - MemoryBank->freeFrames));
    //~ }
    
    
    //~ int* empties = malloc(sizeof(int)*framesNecessary);
    //~ int emptyIndex = 0;
    
    //~ int i;
    //~ for(i = 0; i < numFrames; i++){
        //~ if (framesNecessary-1 == emptyIndex){
            //~ break;
        //~ }
        
        //~ if (MemoryBank->frames[i]){ //go to each string and check if it's empty
            //~ if (MemoryBank->frames[i][0] == '\0'){
                //~ empties[emptyIndex] = i;
            //~ }
        //~ }
    //~ }
    
    //~ return empties;
//~ }

//store(Memory* MemoryBank, string filename, int bytes, string data)
//read(Memory* MemoryBank, int offset, int length)
//deleteOldest(Memory* MemoryBank, int numToFree)

//~ int FileInCache(char * FileName, struct Memory * MemoryBank){
	//~ int i;
	//~ for(i=0;i<MemoryBank->numFiles;i++){
		//~ if(strcmp(MemoryBank->Fille
		
	//~ }
	//~ return 0;
//~ }

//------------------------------------------------------------



int main( int argc, char *argv[] ){
	struct Memory *  MemoryBank = CreateMemory();
	return EXIT_SUCCESS;
}
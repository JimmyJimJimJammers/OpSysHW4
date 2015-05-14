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
	int bytesincache; //Says the amount of bytes in cache
	clock_t PageTableUseTime;
};

struct Memory{ 
	char ** frames;
	struct PageTable ** AllPageTables;
	int freeFrames;
	int numPageTables;
};

void deletecache(char * filename, struct Memory * MemoryBank){
	int i;
	for(i=0;i<MemoryBank->numPageTables;i++){
		if(strcmp(MemoryBank->AllPageTables[i]->Name,filename)==0){
			for(i=0;i<numPages;i++){
				if(MemoryBank->AllPageTables[i]->PageNumber[i] == -1){
					continue;
				}
				else{
					MemoryBank->frames[MemoryBank->AllPageTables[i]->MemoryLocation[i]] = "-1";
					MemoryBank->freeFrames = MemoryBank->freeFrames - 1;
				}
			}
		}
	}
	for (i = 0; i < MemoryBank->numPageTables; i++){
		if(strcmp(MemoryBank->AllPageTables[i]->Name,MemoryBank->AllPageTables[i]->Name) == 0){
			int j;
			for (j = 0; j < MemoryBank->numPageTables; j++){
				MemoryBank->AllPageTables[j] = MemoryBank->AllPageTables[j+1];
			}
			MemoryBank->numPageTables = MemoryBank->numPageTables-1;
		}
	}
}
int main(){
	
	return 0;
}
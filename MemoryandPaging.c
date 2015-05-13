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
	int bytesincache; //Says the amount of bytes in cache
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
	int i;
	for(i=0;i<numFrames;i++){
		MemoryBank->frames[i] = "-1";
	}
	MemoryBank->AllPageTables = malloc(AllPageTablesSize);
	MemoryBank->freeFrames = numFrames;
	MemoryBank->numPageTables = 0;
	return MemoryBank;
}

//~ int findMaxElement(struct Memory* MemoryBank, int* indices){ //finds max element in an array and returns that index
    //~ int maxValIndex = INT_MAX;
//~ }

void Merge(struct PageTable **pt, struct PageTable **Lpt, int leftCount, struct PageTable **Rpt, int rightCount) {
    int i,j,k;
    i = 0; j = 0; k =0;
    while(i<leftCount && j< rightCount) {
        if(Lpt[i]->PageTableUseTime  > Rpt[j]->PageTableUseTime) pt[k++] = Lpt[i++];
        else pt[k++] = Rpt[j++];
    }
    while(i < leftCount) pt[k++] = Lpt[i++];
    while(j < rightCount) pt[k++] = Rpt[j++];
}
void MergeSort(struct PageTable **pt, int n) {
    int mid,i;
    struct PageTable **L = malloc(sizeof(struct PageTable*)*((n/2) + 1));
    struct PageTable **R = malloc(sizeof(struct PageTable*)*((n/2) + 1));
    if(n < 2) return;
    
    mid = n/2; 
    L = malloc(mid*sizeof(struct PageTable));
    R = malloc((n- mid)*sizeof(struct PageTable));
    
    for(i = 0;i<mid;i++) L[i] = pt[i];
    for(i = mid;i<n;i++) R[i-mid] = pt[i]; 
    
    MergeSort(L,mid); 
    MergeSort(R,n-mid); 
    Merge(pt,L,mid,R,n-mid);
    free(L);
    free(R);
}

void DeleteOldest(struct Memory* MemoryBank, int deletions){
	int removedframes;
	struct PageTable ** Temp = calloc(MemoryBank->numPageTables,sizeof(struct PageTable));
	int i;
	while(removedframes != deletions || removedframes < deletions){
		for (i = 0; i < MemoryBank->numPageTables; i++){
			Temp[i] = MemoryBank->AllPageTables[i];
		}
		MergeSort(Temp, MemoryBank->numPageTables);
		for(i=0;i<numPages;i++){
			if(Temp[0]->PageNumber[i] == -1){
				break;
			}
			else{
				//Delete it from cache
				MemoryBank->frames[Temp[0]->MemoryLocation[i]] = "-1";
				Temp[0]->PageNumber[i] = -1;
				Temp[0]->MemoryLocation[i] = -1;
				MemoryBank->freeFrames = MemoryBank->freeFrames - 1;
				removedframes = removedframes + 1;
			}
		}
		for (i = 0; i < MemoryBank->numPageTables; i++){
			if(strcmp(Temp[0]->Name,MemoryBank->AllPageTables[i]->Name) == 0){
				int j;
				for (j = 0; j < MemoryBank->numPageTables; j++){
					MemoryBank->AllPageTables[j] = MemoryBank->AllPageTables[j+1];
				}
				MemoryBank->numPageTables = MemoryBank->numPageTables-1;
			}
		}
	}
}

int* findEmpty(struct Memory* MemoryBank, int framesNecessary){
	if (framesNecessary > MemoryBank->freeFrames){
		//Find the first ones in memory
	}
	if (framesNecessary < MemoryBank->freeFrames){
		//delete that many files
		DeleteOldest(MemoryBank, framesNecessary - MemoryBank->freeFrames);
	}
	int * framesreturned = calloc(1024,sizeof(int));
	int size;
	size = 0;
	int i;
	for(i=0;i<numFrames;i++){
		if(strcmp(MemoryBank->frames[i],"-1")==0){
			framesreturned[size] = i;
			size = size + 1;
		}
		if(size == framesNecessary){
			break;
		}
	}
	return framesreturned;
}

//------------------------------------------------------------



int main( int argc, char *argv[] ){
	struct Memory *  MemoryBank = CreateMemory();
	return EXIT_SUCCESS;
}
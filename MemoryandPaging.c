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
#define maxData 50000

struct PageTable
{
	char * Name;
	int * PageNumber;      //array of page number
	int * MemoryLocation;  //indices stored parallel to PageNumber
    
	int offset;
	int bytesincache; //Says the amount of bytes in cache
	clock_t PageTableUseTime;
};

struct Memory{ 
	char ** frames;
    int * frameBytes;
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
    frameBytes = 0;
	return MemoryBank;
}

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

//function for returning full information from cache
char* returnFull(PageTable* pt, struct Memory* MemoryBank, int offset, int length, int miguel)
{
    char * ret = malloc(maxData*sizeof(char));
    
    //find start point
    int index = 0;
    int max = INT_MAX;
    int i;
    for (i = 0; i < numPages; i++)
    {
        if (pt->PageNumber < max)
        {
            index = i;
            max = pt->PageNumber;
        }
    }
    
    int sentBytes = 0;
    while (sentBytes < length-offset)
    {
        //send message SENT ACK frameBytes to client
        //************************send some more crap************************
        strcat(ret, MemoryBank->frames[pt->[index]]);
        sentBytes += MemoryBank->framesBytes[pt->[index]];
    }
    
    pt->PageTableUseTime = clock();
    
    return ret;
}

//start of the find is missing
char* returnPartialStart(PageTable* pt, struct Memory* MemoryBank, int offset, int length, int miguel)
{
    char * ret = malloc(maxData*sizeof(char));
    
    //find start point
    int index = 0;
    int max = INT_MAX;
    int i;
    for (i = 0; i < numPages; i++)
    {
        if (pt->PageNumber < max)
        {
            index = i;
            max = pt->PageNumber;
        }
    }
    
    //figure out how many frames are necessary to add to the start
    int startDifference = pt->offset - offset;
    
    if (startDifference % frameSize == 0) // if it divides evenly don't add one
    {
        startDifference = startDifference/frameSize;
    }
    else
    {
        startDifference = (startDifference/frameSize)+1;
    }
    
    //figure out how many frames are necessary to add to the end
    int endDifference = offset + length - pt->offset + pt->bytesincache;
    if (endDifference % frameSize == 0) // if it divides evenly don't add one
    {
        endDifference = startDifference/frameSize;
    }
    else
    {
        endDifference = (startDifference/frameSize)+1;
    }
    
    int startTemp = startDifference;
    int endTemp = (startDifference + numPages)-1;
    
    char ** temp[(endTemp-startTemp)+1];// = malloc((endTemp - startTemp)*sizeof(char*));
    
    //go through and save all the information we have cached on the desired data and store it in a temp
    for (i = 0; i < (endTemp-startTemp)+1; i++)
    {
        temp[i] = MemoryBank->frames[pt->PageNumber[index + i]];
    }
    
    //go get the start information from dir
    char** startStuff = GetDirDat(offset, startDifference, pt->Name);
    
    //go get the end information from dir
    char** endStuff = GetDirDat(endTemp, endDifference, pt->Name);
    
    
    int startBytesWritten = 0;
    int cacheBytesWritten = 0;
    int endBytesWritten = 0;
    
    i = 0;
    int j, k, l = 0;
    //now go through all the stuff we don't have at the start, then the stuff we do have, then the stuff we don't have at the end, and cache them/return them
    while (cacheBytesWritten + startBytesWritten + endBytesWritten < length-offset)
    {
        if (startBytesWritten < startDifference)
        {
            strcat(ret,  startStuff[i])
            startBytesWritten += 999999;
        }
        else if(cacheBytesWritten < length-offset)
        {
            strcat(ret, temp[j]);
            cacheBytesWritten += 999999;
        }
        else if(endBytesWritten < endDifference)
        {
            strcat(ret, endStuff[j]);
            endBytesWritten += 9999999;
        }
        
        strcat(ret, MemoryBank->frames[pt->[index]]);
        sentBytes += MemoryBank->framesBytes[pt->[index]];
    }
    
    
    //decrement the index variable (wrap around if -1)
    index--;
    if (index == -1)
    {
        index = numPages-1;
    }
    
    int sentBytes = 0;
    while (sentBytes < length-offset)
    {
        //send message SENT ACK frameBytes to client
        //************************send some more crap************************
        strcat(ret, MemoryBank->frames[pt->[index]]);
        sentBytes += MemoryBank->framesBytes[pt->[index]];
    }
    
    pt->PageTableUseTime = clock();
    
    return ret;
}

//end of the find is missing
char* returnPartialEnd(PageTable* pt, struct Memory* MemoryBank, int offset, int length, int miguel)
{
    char * ret = malloc(maxData*sizeof(char));
    
    //find start point
    int index = 0;
    int max = INT_MAX;
    int i;
    for (i = 0; i < numPages; i++)
    {
        if (pt->PageNumber < max)
        {
            index = i;
            max = pt->PageNumber;
        }
    }
    
    int sentBytes = 0;
    while (sentBytes < length-offset)
    {
        //send message SENT ACK frameBytes to client
        //************************send some more crap************************
        strcat(ret, MemoryBank->frames[pt->[index]]);
        sentBytes += MemoryBank->framesBytes[pt->[index]];
    }
    
    pt->PageTableUseTime = clock();
    
    return ret;
}

//Both start and end are missing
char* returnPartialStartEnd(PageTable* pt, struct Memory* MemoryBank, int offset, int length, int miguel)
{
    char * ret = malloc(maxData*sizeof(char));
    
    //find start point
    int index = 0;
    int max = INT_MAX;
    int i;
    for (i = 0; i < numPages; i++)
    {
        if (pt->PageNumber < max)
        {
            index = i;
            max = pt->PageNumber;
        }
    }
    
    int sentBytes = 0;
    while (sentBytes < length-offset)
    {
        //send message SENT ACK frameBytes to client
        //************************send some more crap************************
        strcat(ret, MemoryBank->frames[pt->[index]]);
        sentBytes += MemoryBank->framesBytes[pt->[index]];
    }
    
    pt->PageTableUseTime = clock();
    
    return ret;
}

char* FindInCache(struct Memory* MemoryBank, char* fileName, int offset, int length, int miguel)
{
    //find pagetable for fileName
    PageTable *pt;
    short inPageTable = 0;
    
    //find pagetable start
    int i;
    for (i = 0; i < MemoryBank->numPageTables; i++)
    {
        if(strcmp(MemoryBank->AllPageTables[i]->Name, fileName) == 0)
        {
            pt = MemoryBank->AllPageTables[i];
            inPageTable = 1;
            break;
        }
    }
    
    //if found it's either in partial or full
    if (inPageTable)
    {
        short startGap = 0;
        short endGap = 0;
        
        //check if the start is before or after requested start
        if (pt->offset > offset) //(if start in cache is after start asked for)
        {
            startGap = 1;
        }
        //check if the end is before or after the requested end
        if (pt->offset + pt->bytesincache < offset + length)
        {
            endGap = 1;
        }
        
        
        
        //full find
        if (!startGap && !endGap)
        {
            
        }
        
        //if partial
        //if we have only a start gap
        if(startGap && !endGap)
        {
            
        }
        else if(!startGap && endGap) //if we only have an end gap
        {
            
        }
        else                        //if we have both a start and end gap
        {
            
        }
        
        
    }
    else //else it's not in there at all
    {
        
    }
    
    
    
    
    
}



int main( int argc, char *argv[] ){
	struct Memory *  MemoryBank = CreateMemory();
	return EXIT_SUCCESS;
}
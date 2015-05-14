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
    
	int offset;       //where we start
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
    MemoryBank->frameBytes = malloc(AllPageTablesSize*sizeof(int));
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
	int removedframes = 0;
	struct PageTable ** Temp = calloc(MemoryBank->numPageTables,sizeof(struct PageTable));
	int i;
	while(removedframes < deletions){
		for (i = 0; i < MemoryBank->numPageTables; i++){
			Temp[i] = MemoryBank->AllPageTables[i];             //THIS IS rearranging actual memorybank FIX THIS*****
		}
		MergeSort(Temp, MemoryBank->numPageTables);
		for(i=0;i<numPages;i++){
			if(Temp[0]->PageNumber[i] == -1){
				continue;
			}
			else{
				//Delete it from cache
				MemoryBank->frames[Temp[0]->MemoryLocation[i]] = "-1";          //THERE SHOULD BE FREE'S HERE
				Temp[0]->PageNumber[i] = -1;
				Temp[0]->MemoryLocation[i] = -1;
				MemoryBank->freeFrames++;
				removedframes = removedframes + 1;
			}
            if (removedframes < deletions) { break; }
		}
        if (removedframes < deletions) { break; }
		for (i = 0; i < MemoryBank->numPageTables; i++){//looking for position of the thing we are deleting in original frames
			if(strcmp(Temp[0]->Name,MemoryBank->AllPageTables[i]->Name) == 0){
				int j;
                
                
                //check if all the pageNums and CacheLocs == -1, if so delete the page table
                //****NEED TO ADD IF STATEMENT******
                
				for (j = 0; j < MemoryBank->numPageTables; j++){ //move all page tables
					MemoryBank->AllPageTables[j] = MemoryBank->AllPageTables[j+1];
				}
				MemoryBank->numPageTables = MemoryBank->numPageTables-1;
			}
		}
	}
}

char ** GetDirDat(int offset, int length, char* fileName);

//given a page table, same data, and a pagenum, store the data in a CacheLoc and link the PageNum to the CacheLoc
void AddToPageTable(struct PageTable* pt, struct Memory* MemoryBank, int pageNum, char* data)
{
    int min = INT_MIN;
    int index = 0;
    
    int i;
    for (i = 0; i < numPages; i++)
    {
        //look for the first -1 you can find.
        if (pt->PageNumber[i] == -1)
        {
            index = i;
            break;
        }
        else if(pt->PageNumber[i] > min) //Otherwise just overwrite the smallest indexed thing
        {
            index = i;
            min = pt->PageNumber[i];
        }
    }
    
    //if the pagenum is -1 and the cacheLoc is -1, find a frame in CacheMem to store stuff
    //WRITE METHOD TO ADD A FILEINFO (AKA A PAGE TABLE) TO THE FRAMES
    
    
    //reassign pt->pageNumber[index] to pageNum(provided)
    
    
    //store actual data in the frame we have reserved
    
    //set the name too just in case
    
    /*think about setting
     int offset;       //where we start
     int bytesincache; //Says the amount of bytes in cache
     */
    
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
char* returnFull(struct PageTable* pt, struct Memory* MemoryBank, int offset, int length, int miguel)
{
    char * ret = malloc(maxData*sizeof(char));
    
    //find start point
    int index = 0;
    int max = INT_MAX;
    int i;
    for (i = 0; i < numPages; i++)
    {
        if (pt->PageNumber[i] < max)
        {
            index = i;
            max = pt->PageNumber[i];
        }
    }
    
    int sentBytes = 0;
    while (sentBytes < length-offset)
    {
        //send message SENT ACK frameBytes to client
        // ************************send some more crap************************
        strcat(ret, MemoryBank->frames[pt->MemoryLocation[index]]);
        sentBytes += MemoryBank->frameBytes[pt->MemoryLocation[index]];
        index++;
    }
    
    pt->PageTableUseTime = clock();
    
    return ret;
}

//start of the find is missing
char* returnPartial(struct PageTable* pt, struct Memory* MemoryBank, int offset, int length, int miguel)
{
    char * ret = malloc(maxData*sizeof(char));
    
    //find start point
    int index = 0;
    int max = INT_MAX;
    int i;
    for (i = 0; i < numPages; i++)
    {
        if (pt->PageNumber[i] < max)
        {
            index = i;
            max = pt->PageNumber[i];
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
    
    char * temp[(endTemp-startTemp)+1];// = malloc((endTemp - startTemp)*sizeof(char*));
    
    //go through and save all the information we have cached on the desired data and store it in a temp
    for (i = 0; i < (endTemp-startTemp)+1; i++)
    {
        temp[i] = MemoryBank->frames[pt->PageNumber[index + i]]; //char* =
    }
    
    //go get the start information from dir
    char** startStuff = GetDirDat(offset, startDifference, pt->Name);
    
    //go get the end information from dir
    char** endStuff = GetDirDat(endTemp, endDifference, pt->Name);
    
    
    int startBytesWritten = 0;
    int cacheBytesWritten = 0;
    int endBytesWritten = 0;
    
    i = 0;
    int j = 0;
    int k = 0;
    int l = index;
    //now go through all the stuff we don't have at the start, then the stuff we do have, then the stuff we don't have at the end, and cache them/return them
    while (cacheBytesWritten + startBytesWritten + endBytesWritten < length-offset)
    {
        if (startBytesWritten < startDifference)
        {
            strcat(ret,  startStuff[i]);
            startBytesWritten += 999999;
            AddToPageTable(pt, MemoryBank, l, startStuff[i]); //given pt, MemoryBank, index, dataToAdd *****IF DOESN"T HAVE PHYSICAL MEMORY LOCATION, MAKE ONE
            i++;
            l++;
            
        }
        else if(cacheBytesWritten < length-offset)
        {
            strcat(ret, temp[j]);
            cacheBytesWritten += 999999;
            AddToPageTable(pt, MemoryBank, l, temp[i]); //given pt, MemoryBank, index, dataToAdd *****IF DOESN"T HAVE PHYSICAL MEMORY LOCATION, MAKE ONE
            j++;
            l++;
        }
        else if(endBytesWritten < endDifference)
        {
            strcat(ret, endStuff[k]);
            endBytesWritten += 9999999;
            AddToPageTable(pt, MemoryBank, l, endStuff[i]); //given pt, MemoryBank, index, dataToAdd *****IF DOESN"T HAVE PHYSICAL MEMORY LOCATION, MAKE ONE
            k++;
            l++;
        }
    }
    
    pt->PageTableUseTime = clock();
    
    return ret;
}

//end of the find is missing
/*char* returnPartialEnd(struct PageTable* pt, struct Memory* MemoryBank, int offset, int length, int miguel)
{
    char * ret = malloc(maxData*sizeof(char));
    
    //find start point
    int index = 0;
    int max = INT_MAX;
    int i;
    for (i = 0; i < numPages; i++)
    {
        if (pt->PageNumber[i] < max)
        {
            index = i;
            max = pt->PageNumber[i];
        }
    }
    
    int sentBytes = 0;
    while (sentBytes < length-offset)
    {
        //send message SENT ACK frameBytes to client
        // ************************send some more crap************************
        strcat(ret, MemoryBank->frames[pt->MemoryLocation[index]]);
        sentBytes += MemoryBank->framesBytes[pt->[index]];
    }
    
    pt->PageTableUseTime = clock();
    
    return ret;
}

//Both start and end are missing
char* returnPartialStartEnd(struct PageTable* pt, struct Memory* MemoryBank, int offset, int length, int miguel)
{
    char * ret = malloc(maxData*sizeof(char));
    
    //find start point
    int index = 0;
    int max = INT_MAX;
    int i;
    for (i = 0; i < numPages; i++)
    {
        if (pt->PageNumber[i] < max)
        {
            index = i;
            max = pt->PageNumber[i];
        }
    }
    
    int sentBytes = 0;
    while (sentBytes < length-offset)
    {
        //send message SENT ACK frameBytes to client
        // ************************send some more crap************************
        strcat(ret, MemoryBank->frames[pt->[index]]);
        sentBytes += MemoryBank->framesBytes[pt->[index]];
    }
    
    pt->PageTableUseTime = clock();
    
    return ret;
}*/

char* FindInCache(struct Memory* MemoryBank, char* fileName, int offset, int length, int miguel)
{
    //find pagetable for fileName
    struct PageTable *pt;
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
    
    
    return returnPartial(pt, MemoryBank, offset, length, 0);//LAST NUMBER IS MIGUEL
    
    //if found it's either in partial or full
    /*if (inPageTable)
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
        
    }*/
    
    
    
    
    
}



int main( int argc, char *argv[] ){
	struct Memory *  MemoryBank = CreateMemory();
	return EXIT_SUCCESS;
}
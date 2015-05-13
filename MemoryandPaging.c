//Matthew DiLandro
//Assignment 4
//Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <time.h>
#include <limits.h>

#define AllFilesSize 1024
#define numFrames 32
#define frameSize 1024
#define numPages 4

struct File{
	char * Name;
	int * PageNumber;      //array of page number
	int * MemoryLocation;  //indices stored parallel to PageNumber
    int offset;
	int ByteLength;
    clock_t fileCreationTime;
    clock_t fileModificationTime;
};

struct Memory{ 
	char ** frames;
	struct File ** AllFiles;
    int freeFrames;
    int numFiles;
};

struct File * CreateFile(){
    struct File * cacheFile = malloc(sizeof(struct File));
    cacheFile->PageNumber = calloc(numPages, sizeof(int));
    cacheFile->MemoryLocation = calloc(numPages, sizeof(int));
    cacheFile->offset = 0;
    cacheFile->ByteLength = 0;
    cacheFile->fileCreationTime = clock();                            //remind miguel to make clock(0);
    cacheFile->fileModificationTime = clock();
    
    return cacheFile;
}

struct Memory * CreateMemory(){
	struct Memory *  MemoryBank = malloc(sizeof(struct Memory));
	MemoryBank->frames = calloc(numFrames, frameSize);
	MemoryBank->AllFiles = malloc(AllFilesSize);
    MemoryBank->freeFrames = numFrames;
    MemoryBank->numFiles = 0;
    
	return MemoryBank;
}

int findMaxElement(struct Memory* MemoryBank, int* indices) //finds max element in an array and returns that index
{
    int maxValIndex = INT_MAX;
}

void DeleteOldest(struct Memory* MemoryBank, int deletions)
{
    int* removals = malloc(sizeof(int)*deletions);
    
    //go through and fill removals with things and record the maximum element of it and the minimum element
    
    //as we look at new things if they are smaller than the max element replace the max element with it, if it is smaller than the min element replace the min element with it and update min
    
    int i;
    for (i = 0; i < MemoryBank->numFiles; i++)
    {
        
    }
}

int* findEmpty(struct Memory* MemoryBank, int framesNecessary)
{
    if (framesNecessary > MemoryBank->numFiles)
    {
        framesNecessary = MemoryBank->numFiles;
    }
    
    if (framesNecessary > MemoryBank->freeFrames)
    {
        //delete that many files
        DeleteOldest(MemoryBank, (framesNecessary - MemoryBank->freeFrames));
    }
    
    
    int* empties = malloc(sizeof(int)*framesNecessary);
    int emptyIndex = 0;
    
    int i;
    for(i = 0; i < numFrames; i++)
    {
        if (framesNecessary-1 == emptyIndex)
        {
            break;
        }
        
        if (MemoryBank->frames[i]) //go to each string and check if it's empty
        {
            if (MemoryBank->frames[i][0] == '\0')
            {
                empties[emptyIndex] = i;
            }
        }
    }
    
    return empties;
}

//store(Memory* MemoryBank, string filename, int bytes, string data)
//read(Memory* MemoryBank, int offset, int length)
//deleteOldest(Memory* MemoryBank, int numToFree)

//------------------------------------------------------------




int main( int argc, char *argv[] )
{
	struct Memory *  MemoryBank = CreateMemory();
	return EXIT_SUCCESS;
}
//Matthew DiLandro
//Assignment 4
//Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#define AllFilesSize 1024
#define numFrames 32
#define frameSize 1024

struct File{
	char * Name;
	int ** PageNumber;
	int ** MemoryLocation;
	int ByteLength;
};

struct Memory{ 
	char ** frames;
	struct File ** AllFiles;
    int freeFrames;
    int numFiles;
    
};

struct Memory * CreateMemory(){
	struct Memory *  MemoryBank = malloc(sizeof(struct Memory));
	MemoryBank->frames = calloc(numFrames, frameSize);
	MemoryBank->AllFiles = malloc(AllFilesSize);
    freeFrames = numFrames;
    numFiles = 0;
	return MemoryBank;
}

int* findEmpty(Memory* MemoryBank, int framesNecessary)
{
    int* empties = malloc(sizeof(int)*framesNecessary);
    int emptyIndex = 0;
    
    int i;
    for(i = 0; i < numFrames; i++)
    {
        if (framesNecessary == emptyIndex)
        {
            break;
        }
        
        if (MemoryBank.frames[i]) //go to each string and check if it's empty
        {
            if (frames[i][0] == '\0')
            {
                empties[emptyIndex] = i;
            }
        }
    }
}

//store(Memory* MemoryBank, string filename, int bytes, string data)
//read(Memory* MemoryBank, int offset, int length)
//deleteOldest(Memory* MemoryBank, int numToFree)

//------------------------------------------------------------




int main( int argc, char *argv[] ){
	struct Memory *  MemoryBank = CreateMemory();
	return EXIT_SUCCESS;
}
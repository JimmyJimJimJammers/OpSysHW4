//Matthew DiLandro
//Assignment 4
//Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AllFilesSize 1024

struct File{
	char * Name;
	int ** PageNumber;
	int ** MemoryLocation;
	int ByteLength;
};

struct Memory{ 
	char ** Frames;
	struct File ** AllFiles;
};

struct Memory * CreateMemory(){
	struct Memory *  MemoryBank = malloc(sizeof(struct Memory));
	MemoryBank->Frames = calloc(32,1024);
	MemoryBank->AllFiles = malloc(AllFilesSize);
	return MemoryBank;
}

int main( int argc, char *argv[] ){
	struct Memory *  MemoryBank = CreateMemory();
	return EXIT_SUCCESS;
}
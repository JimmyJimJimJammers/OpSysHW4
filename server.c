#include <stdio.h>
#include <string.h> /* memset() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/stat.h>
#include <error.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <limits.h>

#define PORT    "8765" /* Port to listen on */
#define BACKLOG     10  /* Passed to listen() */
#define BUFFER_SIZE 1024 /* Buffer size for data 6144*/
#define storeFolderName "storage" /* Storage folder name, add '.' when finished*/

#define AllPageTablesSize 1024
#define numFrames 32
#define frameSize 1024
#define frameSizeString "1024"
#define numPages 4
#define maxData 10240

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

struct Chunky{
    char **data;
    int *bytes;
};

struct Chunky * GetDirDat(int offset,int length, char * name){
    if(length - offset <= 0){
        return NULL;
    }
    int FramesNeeded;
    if(length <= frameSize){
        FramesNeeded = 0;           //incrementer
    }
    else{
        FramesNeeded = (length/frameSize); //this is how many frames are needed (so like say you need 2049 bytes, you need 3 frames)
    }
    //printf("\n%d\n",FramesNeeded);
    int LastFrameBytes = (length%frameSize);            //this is the remainder after all the 1024 frames
    //printf("\n%d\n",LastFrameBytes);                  //this is the array of string length
    struct Chunky * ret = malloc(sizeof(struct Chunky));    //this is our return
    (*ret).data = malloc(1024*sizeof(char *));       //holds the array of strings
    (*ret).bytes = malloc(1024*sizeof(int));
    FILE * FD;
    FD = fopen(name, "r");                                  //the file

            /**********************DO LSTAT ON FILE TO CHECK IF LENGTH IS TOO LARGE FOR ACTUAL FILE SIZE-OFFSET**************/
    //printf("XXXXXXXXXXXXXXXX\n");
    if(FD){
        fseek(FD,offset,SEEK_SET);                           //look for offset point in file
        int i;
        for(i=0;i<FramesNeeded;i++){
            char * Buffer = malloc(frameSize*sizeof(char));      //bytes for array of bytes
            fread(Buffer, 1, frameSize, FD);
            (*ret).data[i] = malloc(frameSize*sizeof(char));
            (*ret).data[i] = Buffer;
            (*ret).bytes[i] = frameSize;
        }
        if(LastFrameBytes != 0){
            char * Boffer = malloc(LastFrameBytes*sizeof(char));
            fread(Boffer, 1, LastFrameBytes, FD);
            (*ret).data[i] = malloc(frameSize*sizeof(char));
            ret->data[i] = Boffer;
            ret->bytes[i] = LastFrameBytes;
        }
    }
    return ret;
}

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

int* findEmpty(struct Memory* MemoryBank, int framesNecessary){
    if (framesNecessary < MemoryBank->freeFrames){
        //Find the first ones in memory
    }
    else if (framesNecessary > MemoryBank->freeFrames){
        //delete that many files
        DeleteOldest(MemoryBank, framesNecessary - MemoryBank->freeFrames);
    }
    int * framesreturned = calloc(framesNecessary,sizeof(int));
    int size = 0;
    int i;
    for(i=0;i<numFrames;i++){
        if(strcmp(MemoryBank->frames[i],"-1")==0)
        {
            framesreturned[size] = i;
            size++;// = size + 1;
        }
        if(size == framesNecessary){
            break;
        }
    }
    return framesreturned;
}

//given a page table, same data, and a pagenum, store the data in a CacheLoc and link the PageNum to the CacheLoc
void AddToPageTable(struct PageTable* pt, struct Memory* MemoryBank, int pageNum, char* data, char* name, int offset, int length)
{
    //check memory for a pagetable with this name
    short exists = 0;
    int ptIndex = 0;
    int i;
    for (i = 0; i < MemoryBank->numPageTables; i++)
    {
        if (strcmp(MemoryBank->AllPageTables[i]->Name, name) == 0)
        {
            ptIndex = i;
            exists = 1;
        }
    }
    
    if (exists && pt != MemoryBank->AllPageTables[ptIndex])//if this page table already exists in memory, then lets set pt to it
    {
        // free(pt->Name);                      ///////**********probs wont be fixed. but issue with frees here. FREEDOM UNITS!
        // free(pt->PageNumber);
        // free(pt->MemoryLocation);
        // free(pt);
        pt = MemoryBank->AllPageTables[ptIndex];
    }
    else//otherwise lets add it to the page tables in memory
    {
        MemoryBank->AllPageTables[MemoryBank->numPageTables] = pt;
        MemoryBank->numPageTables++;
    }
    
    
    
    int min = INT_MIN;
    int index = 0;
    int* memLocs;// = malloc(sizeof(int));
    
    //int i;
    for (i = 0; i < numPages; i++)
    {
        //look for the first -1 you can find.
        if (pt->PageNumber[i] == -1)
        {
            //if the pagenum is -1 and the cacheLoc is -1, find a frame in CacheMem to store stuff
            index = i;
            memLocs = findEmpty(MemoryBank, 1);   //this is broke as shit, fucking broken... really broken. SERIOUSLY FUCKING BROKEN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            
            break;
        }
        else if(pt->PageNumber[i] > min) //Otherwise just overwrite the smallest indexed thing
        {
            /* Transferred 1024 bytes from offset 6144
             Allocated page 5 to frame 1
             Allocated page 9 to frame 1 (replaced page 5)
              Deallocated frame 0 */
            index = i;
            min = pt->PageNumber[i];
        }
    }
    
    printf("[Thread %lu] Allocated page %d to frame %d (replaced page %d)\n", pthread_self(), pageNum, memLocs[0], pt->PageNumber[index]);
    //reassign pt->pageNumber[index] to pageNum(provided)
    /*DO I NEED TO SET NAME HERE? SHOULD I HAVE THAT PASSED AS AN ARGUMENT?********/
    pt->Name = name;                        //set the name too just in case
    pt->PageNumber[index] = pageNum;
    pt->MemoryLocation[index] = memLocs[0]; //store actual data in the frame we have reserved
    pt->offset = offset;
    pt->bytesincache = length;
    pt->PageTableUseTime = clock();
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
    struct Chunky *startStuff = GetDirDat(offset, startDifference, pt->Name);
    
    //go get the end information from dir
    struct Chunky *endStuff = GetDirDat(endTemp, endDifference, pt->Name);
    
    
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

        /* Transferred 1024 bytes from offset 6144
             Allocated page 5 to frame 1
              Deallocated frame 0 */

        if (startBytesWritten < startDifference)
        {
            strcat(ret,  (*startStuff).data[i]);
            startBytesWritten += 999999;
            AddToPageTable(pt, MemoryBank, l, (*startStuff).data[i], pt->Name, offset, length); //given pt, MemoryBank, index, dataToAdd *****IF DOESN"T HAVE PHYSICAL MEMORY LOCATION, MAKE ONE
            i++;
            l++;
            
        }
        else if(cacheBytesWritten < length-offset)
        {
            strcat(ret, temp[j]);
            cacheBytesWritten += 999999;
            AddToPageTable(pt, MemoryBank, l, temp[i], pt->Name, offset, length); //given pt, MemoryBank, index, dataToAdd *****IF DOESN"T HAVE PHYSICAL MEMORY LOCATION, MAKE ONE
            j++;
            l++;
        }
        else if(endBytesWritten < endDifference)
        {
            strcat(ret, (*endStuff).data[k]);
            endBytesWritten += 9999999;
            AddToPageTable(pt, MemoryBank, l, (*endStuff).data[i], pt->Name, offset, length); //given pt, MemoryBank, index, dataToAdd *****IF DOESN"T HAVE PHYSICAL MEMORY LOCATION, MAKE ONE
            k++;
            l++;
        }
    }
    
    pt->PageTableUseTime = clock();
    
    return ret;
}

struct Data {
    int newsocket;
    pthread_mutex_t *mut;
    struct Memory *memoryBank;
};

void *handle(void *args)
{   
    struct Data *d = (struct Data *)args;
    int newsock = (*d).newsocket;
    pthread_mutex_t *mutex = (*d).mut;
    struct Memory *memoryBank = (*d).memoryBank;

    char *buffer = malloc(BUFFER_SIZE*sizeof(char));//[ BUFFER_SIZE ];
    while ( 1 ){
        /* can also use read() and write() */
        int n = recv( newsock, buffer, BUFFER_SIZE - 1, 0 ); // BLOCK
        if ( n < 0 )
        {
            perror( "recv()" );
            break;
        }
        else if ( n == 0 )
        {
            printf("[Thread %lu] Client closed its socket....terminating\n", (unsigned long)pthread_self());
            break;
        }
        else {
            char *token;                           
            char **str = malloc(sizeof(char *));
            char substr[4];
            strncpy(substr, buffer, 4);
            substr[3]='\0';
            token = strtok_r(buffer, " \n", str);     
            buffer = *str;

            if(strcmp(token, "STORE") == 0) {
                pthread_mutex_lock(mutex);
                //STORE <filename> <bytes>\n<file-contents>
                char *filename;
                char *numBytes;
                filename = strtok_r(NULL, " ", str);
                buffer = *str;
                numBytes = strtok_r(NULL, "\n", str);
                buffer = *str;
                printf( "[Thread %lu] Rcvd: STORE %s %s\n", (unsigned long)pthread_self(), filename, numBytes);
                char *buffyTheBitSlayer=malloc(atoi(numBytes)*sizeof(char *));
                if(atoi(numBytes) + strlen(filename) + strlen(numBytes) + 8 > BUFFER_SIZE){
                    int angel = atoi(numBytes)-8-strlen(filename)-strlen(numBytes);
                    char *vampire = malloc(angel*sizeof(char *));
                    strcat(buffyTheBitSlayer, buffer);
                    n = recv( newsock, vampire, angel, 0 );
                    strcat(buffyTheBitSlayer, vampire);
                }
                else{
                    strcat(buffyTheBitSlayer, buffer);
                }
                struct stat s;
                int err = stat(filename, &s);
                if (-1 == err) {
                    int w = open(filename, O_CREAT | O_RDWR, 0666 );
                    int nB = atoi(numBytes);
                    write(w, buffyTheBitSlayer, nB);
                    close(w);
                    printf("[Thread %lu] Transferred file (%d bytes)\n", (unsigned long)pthread_self(), atoi(numBytes));
                    printf( "[Thread %lu] Sent: ACK\n", (unsigned long)pthread_self());
                    n = send(newsock, "ACK\n", 4, 0);
                    if ( n != 4 ) {
                      perror( "store send() failed" );
                    }
                }
                else{
                    printf( "[Thread %lu] Sent: ERROR: FILE EXISTS\n", (unsigned long)pthread_self());
                    n = send(newsock, "ERROR: FILE EXISTS\n", 19, 0);
                    if ( n != 19 ) {
                      perror( "store error send() failed" );
                    }

                }
                pthread_mutex_unlock(mutex);
            }


            else if(strcmp(token, "READ") == 0) {
                pthread_mutex_lock(mutex);
                //READ <filename> <byte-offset> <length>\n
                char *filename;
                char *byteOffset;
                char *length;
                filename = strtok_r(NULL, " ", str);
                buffer = *str;
                byteOffset = strtok_r(NULL, " ", str);
                buffer = *str;
                length = strtok_r(NULL, "\n", str);
                buffer = *str;

                printf( "[Thread %lu] Rcvd: READ %s %s %s\n", (unsigned long)pthread_self(), filename, byteOffset, length);
                struct stat s;
                int err = stat(filename, &s);
                if (-1 != err) {

                    /*READ HERE*/
                    struct Chunky *monkey = GetDirDat(atoi(byteOffset), atoi(length), filename);
                    int i=0;
                    while((*monkey).bytes[i] == 1024){
                        //printf("MONKEY OUTPUT %s\n", (*monkey).data[0]);
                        char *ackpacket = malloc(5+strlen(frameSizeString)+frameSize*sizeof(char));
                        strcpy(ackpacket, "ACK ");
                        strcat(ackpacket, frameSizeString);
                        strcat(ackpacket, "\n");

                        /* ACK <bytes>\n<file-contents> */
                        struct PageTable *pt= CreatePageTable();
                        (*pt).Name = filename;
                        AddToPageTable(pt, memoryBank, i, (*monkey).data[i], filename, atoi(byteOffset), atoi(length));
                        printf( "[Thread %lu] Sent: %s", (unsigned long)pthread_self(), ackpacket);
                        strcat(ackpacket, (*monkey).data[i]);
                        n = send(newsock, ackpacket, strlen( ackpacket ), 0);
                        if ( n != strlen( ackpacket ) ) {
                          perror( "read send() failed" );
                        }
                        i++;
                    }
                    if((*monkey).bytes[i] != 0){
                        char num[100];
                        sprintf(num, "%d", (*monkey).bytes[i]);
                        char *ackpacket = malloc(6+strlen(num)+(*monkey).bytes[i]*sizeof(char));
                        strcpy(ackpacket, "ACK ");
                        strcat(ackpacket, num);
                        strcat(ackpacket, "\n");

                        /* ACK <bytes>\n<file-contents> */
                        struct PageTable *pt= CreatePageTable();
                        (*pt).Name = filename;
                        AddToPageTable(pt, memoryBank, i, (*monkey).data[i], filename, atoi(byteOffset), atoi(length));
                        printf( "[Thread %lu] Sent: %s", (unsigned long)pthread_self(), ackpacket);
                        strcat(ackpacket, (*monkey).data[i]);
                        n = send(newsock, ackpacket, strlen( ackpacket ), 0);
                        if ( n != strlen( ackpacket ) ) {
                          perror( "read send() failed" );
                        }
                    }
                }
                else{
                    printf( "[Thread %lu] Sent: ERROR: NO SUCH FILE\n", (unsigned long)pthread_self());
                    n = send(newsock, "ERROR: NO SUCH FILE\n", 20, 0);
                    if ( n != 20 ) {
                      perror( "read error send() failed" );
                    }
                }
                pthread_mutex_unlock(mutex);
            }


            else if(strcmp(token, "DELETE") == 0) {
                pthread_mutex_lock(mutex);
                char *filename;
                filename = strtok_r(NULL, "\n", str);
                buffer = *str;
                printf( "[Thread %lu] Rcvd: DELETE %s\n", (unsigned long)pthread_self(), filename);
                char *fname = malloc(sizeof(filename)-1);
                strncpy(fname, filename, strlen(filename)-1);
                strcat(fname, "\0");
                struct stat s;
                int err = stat(fname, &s);
                if (0 == err) {
                    int x = remove(fname);
                    free(fname);
                    if(x == 0){
                        printf( "[Thread %lu] Deleted %s file\n", (unsigned long)pthread_self(), fname);
                        printf( "[Thread %lu] Sent: ACK\n", (unsigned long)pthread_self());
                        n = send(newsock, "ACK\n", 4, 0);
                        if ( n != 4 ) {
                          perror( "delete send() failed" );
                        }
                    }
                    
                    else{
                        printf( "[Thread %lu] Sent: ERROR: ERROR DELETING\n", (unsigned long)pthread_self());
                        n = send(newsock, "ERROR: ERROR DELETING\n", 22, 0);
                        if ( n != 22 ) {
                          perror( "delete error send() failed" );
                        }
                    }
                }
                else{
                    printf( "[Thread %lu] Sent: ERROR: NO SUCH FILE\n", (unsigned long)pthread_self());
                    n = send(newsock, "ERROR: NO SUCH FILE\n", 20, 0);
                    if ( n != 20 ) {
                      perror( "delete error send() failed" );
                    }
                }
                pthread_mutex_unlock(mutex);
            }


            else if(strcmp(substr, "DIR\0") == 0) {
                printf( "[Thread %lu] Rcvd: DIR\n", (unsigned long)pthread_self());

                /* <number-of-files>\n<filename1>\n<filename2>\netc.\n */

                DIR *dir;
                struct dirent *ent;
                char cwd[ BUFFER_SIZE ];
                getcwd(cwd, sizeof(cwd));
                char *files = malloc(BUFFER_SIZE*sizeof(char));
                int numFiles = 0;
                if ((dir = opendir (cwd)) != NULL) {
                  /* print all the files and directories within directory */
                  while ((ent = readdir (dir)) != NULL) {
                    if( strcmp( ent->d_name, "." ) == 0 || 
                        strcmp( ent->d_name, ".." ) == 0 ) {
                      continue;
                    }
                    strcat(files, ent->d_name);
                    strcat(files, "\n");
                    numFiles++;
                  }
                  closedir (dir);
                } else {
                  /* could not open directory */
                  perror ("error opening directory");
                }
                if(numFiles > 0){
                    n = send(newsock, "Number of Files: ", strlen("Number of Files: "), 0);
                    if ( n != strlen("Number of Files: ") ) {
                      perror( "delete error send() failed" );
                    }
                    char num[100];
                    sprintf(num, "%d", numFiles);
                    n = send(newsock, num, strlen(num), 0);
                    if ( n != strlen(num) ) {
                      perror( "delete error send() failed" );
                    }
                    n = send(newsock, "\n", 1, 0);
                    if ( n != 1 ) {
                      perror( "delete error send() failed" );
                    }
                    n = send(newsock, files, strlen(files), 0);
                    if ( n != strlen(files) ) {
                      perror( "delete error send() failed" );
                    }
                    free(files);
                }
                else{
                    n = send(newsock, "Number of Files: 0\n", strlen("Number of Files: 0\n"), 0);
                    if ( n != strlen("Number of Files: 0\n") ) {
                      perror( "delete error send() failed" );
                    }
                }

            }

            else {
                printf("[Thread %lu] Sent: ERROR: Unknown command: %s\n", (unsigned long)pthread_self(), token);
                n = send(newsock, "ERROR: Unknown command\n", 23, 0);
                if ( n != 23 ) {
                  perror( "delete send() failed" );
                }
            }
        }
    }

    // /* send(), recv(), close() */
    return NULL;
}

int main(void)
{

    /* Check for .storage foler and if not there create it */
    char cwd[ BUFFER_SIZE ];
    getcwd(cwd, sizeof(cwd));
    struct stat s;
    int err = stat(storeFolderName, &s);
    if (-1 == err) {
        if (ENOENT == errno) {
            mkdir(storeFolderName, 0777);
            strcat(cwd, "/");
            strcat(cwd, storeFolderName);
            chdir(cwd);
        } else {
            perror("stat");
            exit(1);
        }
    } else {
        if (S_ISDIR(s.st_mode)) {
            strcat(cwd, "/");
            strcat(cwd, storeFolderName);
            chdir(cwd);
        } else {
            printf("ERROR: error creating storage directory\n");
            fflush(NULL);
        }
    }

    int sock;
    pthread_t thread;
    struct addrinfo hints, *res;
    int reuseaddr = 1; /* True */
    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));

    /* Get the address info */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    /* Create the socket */
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    /* Enable the socket to reuse the address */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1) {
        perror("setsockopt");
        return 1;
    }

    /* Bind to the address */
    if (bind(sock, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        return 0;
    }

    freeaddrinfo(res);

    /* Listen */
    if (listen(sock, BACKLOG) == -1) {
        perror("listen");
        return 0;
    }
    printf("Listening on port %s\n", PORT);
    /* Main loop */
    while (1) {
        size_t size = sizeof(struct sockaddr_in);
        struct sockaddr_in their_addr;
        int newsock = accept(sock, (struct sockaddr *)&their_addr, (socklen_t *)&size);
        if (newsock == -1) {
            perror("accept");
        }
        else {
            struct Data *d = malloc(32*sizeof(struct Data));
            (*d).newsocket=newsock;
            (*d).mut = mutex;
            struct Memory *memoryBank = CreateMemory();
            (*d).memoryBank = memoryBank;

            printf("Received incoming connection from %s\n", inet_ntoa(their_addr.sin_addr));
            if (pthread_create(&thread, NULL, handle, d) != 0) {
                fprintf(stderr, "Failed to create thread\n");
            }
        }
    }

    close(sock);
    //printf("\nTotal uptime for server: %lu\n", total_t );
    return 0;
}

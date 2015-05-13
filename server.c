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

#define PORT    "8765" /* Port to listen on */
#define BACKLOG     10  /* Passed to listen() */
#define BUFFER_SIZE 6144 /* Buffer size for data */
#define storeFolderName "storage" /* Storage folder name, add '.' when finished*/

void *handle(void *pnewsock)
{
    int newsock = *((int *)pnewsock);
    char *buffer = malloc(BUFFER_SIZE*sizeof(char));//[ BUFFER_SIZE ];
    while ( 1 )
     {
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
        buffer[n] = '\0';
        //strcpy(buffer, "STORE asshat 20 \nlkjsaldkjsaldkjsa\n");
        char *token;                           
        char **str = malloc(sizeof(char *));
        char substr[4];
        strncpy(substr, buffer, 4);
        substr[3]='\0';
        token = strtok_r(buffer, " \n", str);     
        buffer = *str;

        if(strcmp(token, "STORE") == 0) {
            //STORE <filename> <bytes>\n<file-contents>
            char *filename;
            char *numBytes;
            filename = strtok_r(NULL, " ", str);
            buffer = *str;
            numBytes = strtok_r(NULL, "\n", str);
            buffer = *str;
            printf( "[Thread %lu] Rcvd: STORE %s %s\n%s", (unsigned long)pthread_self(), filename, numBytes, buffer);
            int w = open(filename, O_CREAT | O_RDWR, 0666 );
            int nB = atoi(numBytes);
            write(w, buffer, nB);
            close(w);

            n = send(newsock, "ACK\n", 4, 0);
            if ( n != 4 ) {
              perror( "store send() failed" );
            }
        }


        else if(strcmp(token, "READ") == 0) {
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
            char ackpacket[1000];
            strcpy(ackpacket, "ACK ");
            int x = atoi(length) - atoi(byteOffset);
            char byt[1000];
            sprintf(byt, "%d", x);
            strcat(ackpacket, byt);
            strcat(ackpacket, "\n");

            /* ACK <bytes>\n<file-contents> */
            n = send(newsock, ackpacket, strlen( ackpacket ), 0);
            if ( n != strlen( ackpacket ) ) {
              perror( "read send() failed" );
            }
        }


        else if(strcmp(token, "DELETE") == 0) {
            char *filename;
            filename = strtok_r(NULL, "\n", str);
            buffer = *str;
            printf( "[Thread %lu] Rcvd: DELETE %s\n", (unsigned long)pthread_self(), filename);



            n = send(newsock, "ACK\n", 4, 0);
            if ( n != 4 ) {
              perror( "delete send() failed" );
            }
        }


        else if(strcmp(substr, "DIR\0") == 0) {
            printf( "[Thread %lu] Rcvd: DIR\n", (unsigned long)pthread_self());

            /* <number-of-files>\n<filename1>\n<filename2>\netc.\n */
        }


        else {
            printf("[Thread %lu] ERROR: Unknown command: %s\n", (unsigned long)pthread_self(), token);
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

    /* Main loop */
    while (1) {
        size_t size = sizeof(struct sockaddr_in);
        struct sockaddr_in their_addr;
        int newsock = accept(sock, (struct sockaddr *)&their_addr, (socklen_t *)&size);
        if (newsock == -1) {
            perror("accept");
        }
        else {
            printf("Received incoming connection from %s\n", inet_ntoa(their_addr.sin_addr));
            if (pthread_create(&thread, NULL, handle, &newsock) != 0) {
                fprintf(stderr, "Failed to create thread\n");
            }
        }
    }

    close(sock);

    return 0;
}

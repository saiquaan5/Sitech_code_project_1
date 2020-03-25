
#include <stdio.h>          /* for printf() and fprintf() */
#include <sys/socket.h>     /* for socket(),connect(),send() and recv() */
#include <stdlib.h>         /* for atoi() and exit() */
#include <string.h>         /* for memset() */
#include <unistd.h>         /* for close() */
#include <sys/types.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 
// #include "jansson.h"
// #include "json/json.h"
#include <iostream>
#include <jsoncpp/json/json.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <stdio.h>      // for printf() and fprintf()
#include <sys/socket.h> // for socket(), bind(), and connect()
#include <arpa/inet.h>  // for sockaddr_in and inet_ntoa()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>		// for memset()
#include <unistd.h>		// for close()
#include <sys/types.h>	// for Socket data types
#include <netinet/in.h> // for IP Socket data types
#include <pthread.h>	// for multithread function


#include <TCP-IP.h

#define MAX 80;



int main(int argc, char *argv[]){

    int servSock, clntSock,fd;
    struct sockaddr_in cli_addr;
    pthread_t threadID;              /* Thread ID from pthread_create() */
    struct ThreadArgs *threadArgs;   /* Pointer to argument structure for thread */
    unsigned int clntLen;            /* Length of client address data structure */

    clntLen = sizeof(cli_addr);

    servSock = CreateTCPServerSocket(PORT);


    if((fd = serialOpen ("/dev/ttyUSB0", 115200)) < 0 )
    {
        fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
    }


    while(1) {

        clntSock = AcceptTCPConnection(servSock);

        getpeername(clntSock, (struct sockaddr *) &cli_addr, &clntLen);

        /* Create separate memory for client argument */
        if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL)
            error("malloc() failed");

        threadArgs -> clntSock = clntSock;

        if (pthread_create(&threadID, NULL, HandleThreadClient, (void *) threadArgs) != 0)
            error("pthread_create() failed");

        printf("\n+ New client[%d][Addr:%s][Port:%d]\n\n", 
            clntSock, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        print("This is main serial");
    }

    close(servSock);
    return 0; 
}
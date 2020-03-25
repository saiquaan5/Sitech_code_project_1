#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdio.h>          /* for printf() and fprintf() */
#include <sys/socket.h>     /* for socket(),connect(),send() and recv() */
#include <stdlib.h>         /* for atoi() and exit() */
#include <string.h>         /* for memset() */
#include <unistd.h>         /* for close() */
#include <sys/types.h> 
#include <netinet/in.h>

// #include <wiringSerial.h>
// #include <wiringPi.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <strings.h> 
// #include <sys/time.h>


// gcc -Wall uart-send.c -o uart-send -lwiringPi
int main()
{
    serialPutchar(1,'c');
}
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(),connect(),send() and recv() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <sys/types.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <iostream>
#include <jsoncpp/json/json.h>
#include <wiringPi.h>
#include <wiringSerial.h>

using namespace std;


typedef struct __attribute__((__packed__ ))
{
    char build[25];
    uint32_t mcu_clock;
    char IMEI[20];
    uint8_t rom_memory;
    char license[20];
    char add[80];
    uint8_t numlamp;
    uint8_t num_group;
    uint8_t mode_control; 
}sys_info_t;

sys_info_t sys_info;

#define MSP_EX_GET_STARTUP              239
#define MSP_EX_INFO                     240
#define MSP_EX_GET_MODE                 241
#define MSP_EX_SET_MODE                 242
#define MSP_EX_DIM_GROUP                243
#define MSP_EX_DIM_ADD                  244
#define MSP_EX_GET_INFO_ID              245
#define MSP_EX_LOGAT_MESSAGE            246
#define MSP_EX_NUM_LAMP                 247

typedef enum _serial_state
{
    IDLE,
    HEADER_START,
    HEADER_M,
    HEADER_ARROW,
    HEADER_SIZE,
    HEADER_CMD,
}_serial_state ;
_serial_state c_state;

uint8_t checksum;
uint8_t indRX;
uint8_t inBuf[255]; 
uint8_t offset;
uint8_t dataSize;
uint8_t cmdMSP;

int fd;

//Truyen 1 byte
void serialize8(uint8_t a)
{
    serialPutchar (fd,a);
    checksum ^= a;
}

//Truyen 2 byte
void serialize16(int16_t a)
{
    static uint8_t t;
    t = a ;
    serialPutchar (fd,t);
    checksum ^= t;
    t = (a >> 8) & 0xff;
    serialPutchar (fd,t);
    checksum ^= t;
}

//Truyen 4 byte
void serialize32(uint32_t a)
{
    static uint8_t t;
    t = a;
    serialPutchar (fd,t);
    checksum ^= t;
    t = a >> 8;
    serialPutchar (fd,t);
    checksum ^= t;
    t = a >> 16;
    serialPutchar (fd,t);
    checksum ^= t;
    t = a >> 24;
    serialPutchar (fd,t);
    checksum ^= t;
}

//
void headSerialResponse(uint8_t err, uint8_t s)
{
    serialize8('$');
    serialize8('M');
    serialize8(err ? '!' : '>');
    checksum = 0; 
    serialize8(s);
    serialize8(cmdMSP);
}

//
void headSerialReply(uint8_t s)
{
    headSerialResponse(0, s);
}

//
void headSerialError(uint8_t s)
{
    headSerialResponse(1, s);
}

//
void tailSerialReply(void)
{
    serialize8(checksum);
}

//
void serializeNames(const char *s)
{
    const char *c;
    for (c = s;*c; c++)
    serialize8(*c);
}
//
uint8_t read8(void)
{
    return inBuf[indRX++] & 0xff;
}
//
uint16_t read16(void)
{
    uint16_t t = read8();
    t += (uint16_t) read8() << 8;
    return t;
}
// 
int16_t readint16(void)//
{
    int16_t temp = (inBuf[indRX++]);
    temp = temp+((inBuf[indRX++])<<8);
    return temp;
}
//
uint32_t read32(void)
{
    uint32_t t = read16();
    t += (uint32_t) read16() << 16;
    return t;
}

//
void send_struct(uint8_t cmd,uint8_t *cb, uint8_t siz){
    cmdMSP = cmd;
    headSerialReply(siz);
    while (siz--)
    {
        serialize8(*cb++);
    }
    tailSerialReply();
}

//
void send_byte(uint8_t cmd,uint8_t data)
{
    cmdMSP = cmd;
    headSerialReply(1);
    serialize8(data);
    tailSerialReply();
}

//
void readstruct(uint8_t *pt, uint8_t size)
{
    uint16_t i = 0; 
    for (i = 0; i < size; i++)
    { 
        *pt = inBuf[indRX++];
        pt++;
    } 
}

char logcat_buff[150];

// Co` ngat
bool flag_info = false;
bool flag_message = false;

void get_data_finish(void)
{
        switch(cmdMSP)
        {
            case MSP_EX_GET_STARTUP:
            readstruct((uint8_t*)&sys_info,sizeof(sys_info_t));
            flag_info =true;
            break;
            case MSP_EX_LOGAT_MESSAGE:
            readstruct((uint8_t *)&logcat_buff,sizeof(logcat_buff));
            flag_message = true; 
            default:
            break;
        }
}

    void serial_get_buffer(void)
    {
        uint8_t c = 0;
        if(serialDataAvail(fd))
        { 
            c = (uint8_t)(serialGetchar(fd)); 
    // cout << "This is check: " << c <<endl;
            if (c_state == IDLE) 
            {
                c_state = (c == '$') ? HEADER_START : IDLE;
                if (c_state == IDLE)
                { 
                }
            } else if (c_state == HEADER_START) 
            {
                c_state = (c == 'M') ? HEADER_M : IDLE;
            } else if (c_state == HEADER_M) 
            {
                c_state = (c == '>') ? HEADER_ARROW : IDLE;
            } else if (c_state == HEADER_ARROW) 
            {
                if (c > 255) 
                { 
                    c_state = IDLE;
                }else
                {
                    dataSize = c;
                    offset = 0;
                    checksum = 0;
                    indRX = 0;
                    checksum ^= c;
                    c_state = HEADER_SIZE; 
                }
            } else if (c_state == HEADER_SIZE) 
            {
                cmdMSP = c;
                checksum ^= c;
                c_state = HEADER_CMD;
            } else if (c_state == HEADER_CMD && offset < dataSize)
            {
                checksum ^= c;
                inBuf[offset++] = c;
            } else if (c_state == HEADER_CMD && offset >= dataSize)
            {
                if (checksum == c) 
                { 
                    get_data_finish(); 
                }
                c_state = IDLE;
            }
    }
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Convert data to strin
// Function designed for chat between client and server. 

int main()
{
    if((fd = serialOpen ("/dev/ttyUSB0", 115200)) < 0 )
    {
        fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
    }
    send_byte(MSP_EX_DIM_GROUP,1);
    while(1)
    {
        serial_get_buffer();
        if (flag_message)
        {
            cout << logcat_buff << endl;
            
            cout << sizeof(logcat_buff) << endl;

            flag_message =false;
        }
    }
    return 0;
}
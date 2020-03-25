#include "MSP.h"
#include "platform/FunctionPointer.h"
#include "platform/mbed_critical.h"



extern Serial  pcCom;


MSP::MSP(Serial& serial,uint32_t baud): _serial(serial)
{
	_serial.baud(baud);
}

void MSP::attach(Callback<void(uint8_t)> func){

core_util_critical_section_enter();
    if(func){
        _function = func;
    }
	_serial.attach(this, &MSP::rxInterrupt, Serial::RxIrq);
	c_state = IDLE;
core_util_critical_section_exit();
}

void MSP::set_baud(uint32_t baud)
{
    _serial.baud(baud);
    c_state = IDLE;
}

//
void MSP::serialize8(uint8_t a)//Truyen 1 byte
{
    _serial.putc(a);
     checksum ^= a;
}

//
void MSP::serialize16(int16_t a)//Truyen 2 byte
{
    static uint8_t t;
    t = a ;
    _serial.putc(t);
    checksum ^= t;
    t = (a >> 8) & 0xff;
    _serial.putc(t);
    checksum ^= t;
}

//
void MSP:: serialize32(uint32_t a)//Truyen 4 byte
{
    static uint8_t t;
    t = a;
    _serial.putc(t);
    checksum ^= t;
    t = a >> 8;
    _serial.putc(t);
    checksum ^= t;
    t = a >> 16;
    _serial.putc(t);
    checksum ^= t;
    t = a >> 24;
    _serial.putc(t);
    checksum ^= t;
}

//
void MSP:: headSerialResponse(uint8_t err, uint8_t s)//Ham dinh dang truyen chuan Multiwii
{
    serialize8('$');
    serialize8('M');
    serialize8(err ? '!' : '>');
    checksum = 0;               // start calculating a new checksum
    serialize8(s);
    serialize8(cmdMSP);
}
//
void MSP:: headSerialReply(uint8_t s)//Ham bao truyen bao nhieu byte
{
    headSerialResponse(0, s);
}

//
void MSP:: headSerialError(uint8_t s)//Ham bao loi truyen 
{
    headSerialResponse(1, s);
}

//
void MSP:: tailSerialReply(void)//Truyen ma checksum
{
    serialize8(checksum);
}


//
void MSP:: serializeNames(const char *s)
{
    const char *c;
    for (c = s;*c; c++)
        serialize8(*c);
}
//
uint8_t MSP:: read8(void)//Read 1 byte
{
	   return inBuf[indRX++] & 0xff;
}
//
uint16_t MSP::read16(void)//Read 2 byte
{
    uint16_t t = read8();
    t += (uint16_t) read8() << 8;
    return t;
}
// 
int16_t MSP::readint16(void)//
{
	int16_t temp = (inBuf[indRX++]);
	temp = temp+((inBuf[indRX++])<<8);
    return temp;
}
//
uint32_t MSP:: read32(void)//Read 4 byte
{
    uint32_t t = read16();
    t += (uint32_t) read16() << 16;
    return t;
}

//
void  MSP:: send_struct(uint8_t cmd,uint8_t *cb, uint8_t siz){
	cmdMSP = cmd;
    headSerialReply(siz);
    while (siz--){
       // pcCom.printf("\r\nDat:%03d",*cb);
        serialize8(*cb++);
    }
	tailSerialReply();
}

//
void MSP::send_byte(uint8_t cmd,uint8_t data){
	cmdMSP = cmd;
	headSerialReply(1);
	serialize8(data);
	tailSerialReply();
}

//
void MSP:: readstruct(uint8_t *pt, uint8_t size){
    uint16_t i = 0;	
    for (i = 0; i < size; i++){				         	
        *pt =	inBuf[indRX++];
        pt++;
    }	
}

uint8_t MSP::get_cmd(void)
{
	return cmdMSP;
}


void MSP::rxInterrupt(void)
{
uint8_t c = 0;
if(_serial.readable()){    
	c = _serial.getc();	
    //pcCom.putc(c);
	if (c_state == IDLE) {
        c_state = (c == '$') ? HEADER_START : IDLE;
        if (c_state == IDLE){           //todo 
        }
	} else if (c_state == HEADER_START) {
        c_state = (c == 'M') ? HEADER_M : IDLE;
    } else if (c_state == HEADER_M) {
        c_state = (c == '>') ? HEADER_ARROW : IDLE;
    } else if (c_state == HEADER_ARROW) {
        if (c > 255) {       		    // now we are expecting the payload size
            c_state = IDLE;
        }else{
			dataSize = c;
			offset = 0;
			checksum = 0;
			indRX = 0;
			checksum ^= c;
			c_state = HEADER_SIZE;      // the command is to follow
		}
	} else if (c_state == HEADER_SIZE) {
        cmdMSP = c;
        checksum ^= c;
        c_state = HEADER_CMD;
    } else if (c_state == HEADER_CMD && offset < dataSize){
        checksum ^= c;
        inBuf[offset++] = c;
    } else if (c_state == HEADER_CMD && offset >= dataSize){
        if (checksum == c) {            //so sanh su lieu checksum							
			//Du lieu nhan ve da dung voi ma checksum
            if(_function)
            {
                _function(offset-1);
            }														
        }
        c_state = IDLE;
    }
}
}

void MSP:: isr_enable(void)
{
    _serial.attach(this, &MSP::rxInterrupt, Serial::RxIrq);
}
void MSP:: isr_disenable(void)
{
    _serial.attach(0);
}

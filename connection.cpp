


//Cac ma lenh gui va nhan cac du lieu
#define MSP_EX_GET_STARTUP         239 //ma yeu cau va nhan du lieu thong tin he thong
#define MSP_EX_SET_POWERSUPPLY     240 //ma dieu khien nguon
#define MSP_EX_SET_MODE            242 //ma set che do lam viec

#define MSP_EX_DIM_GROUP           243 //ma dim theo group
#define MSP_EX_DIM_ADD             244 //ma dim dia theo dia chi
#define MSP_EX_GET_INFO_ID         245 //Lay thong tin cua den nao do
#define MSP_EX_LOGCAT_MESSAGE      246 //Ma lenh hien thong bao string



/**
 * Goi du lieu luu info cua he thong
*/
typedef struct __attribute__((packed)) {
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


/**
 * dieu khien cong suat den
*/
typedef struct __attribute__((packed)) {
  uint8_t idx;//dia chi den hoac nhom . Den 1 den xx den. Nhom 1-10
  uint8_t percent;//0-100% do sang den
}manual_packet_t;


//Bien luu thong tin 
sys_info_t  sys_info;
manual_packet_t manual_packet;


//Gui yeu cau lay du lieu info
com.send_byte(MSP_EX_GET_STARTUP,1);




//Khi gui yeu cau lay info bo DEV se gui lai struc du lieu
//Ham nhan ma lenh nhan 
//MSP_EX_GET_STARTUP
com.readstruct((uint8_t*)&sys_info,sizeof(sys_info_t));


//Ma lenh set mode lam viec cho bo dieu khien

com.send_byte(MSP_EX_SET_MODE,0);// Tra bo dieu khien ve che do tu do
com.send_byte(MSP_EX_SET_MODE,1);// yeu cau vao che do Manual
com.send_byte(MSP_EX_SET_MODE,2);// yeu cau vao che do auto



        // char* data = get_json_update(&sys_info);
            
            // if (data) {
            //     write(sockfd, data, strlen(data));
            //     printf("Update complete\n");
            //     free(data);
            // }


//Ma lenh dieu khien nguon
com.send_byte(MSP_EX_SET_POWERSUPPLY,0);// yeu cau tat nguon
com.send_byte(MSP_EX_SET_POWERSUPPLY,0);// yeu cau mo nguon


//yeu cau lay thong tin cua 1 den nao do
com.send_byte(MSP_EX_GET_INFO_ID,ID_DEN);
//Sau khi yeu cau DEV se tra 1 string co thong tin den
/*
Lamp ID:4
Lamp is 1.0,
Driver 143.000V,
Brightness 100 ,
Led Voltage 134.400V
*/



//Lenh gui string tu DEV va ras
//Bo dem du lieu nhan
char logcat_buff[150];
//Ma nhan
MSP_EX_LOGCAT_MESSAGE
//lenh nhan tu ras
com.readstruct((uint8_t*)&logcat_buff,sizeof(logcat_buff));



//Lenh dim dia chi den tu  ras
manual_packet.idx = dia_chi_den;
manual_packet.percent  = Do_sang_muon_dim;
com.send_struct(MSP_EX_DIM_ADD,(uint8_t*)&manual_packet,sizeof(manual_packet_t));




//Lenh dim group den tu  ras
manual_packet.idx = group_den;
manual_packet.percent  = Do_sang_muon_dim;
com.send_struct(MSP_EX_DIM_GROUP,(uint8_t*)&manual_packet,sizeof(manual_packet_t));


#include <string>
#include <iostream>
#include "json/json.h"

using namespace std;

int main(int argc, _TCHAR* argv[])
{
  Json::Reader reader;
  Json::Value root;

  string json = "{\"call\": \"KF6GPE\",\"type\":\"l\",\"time\":
  \"1399371514\",\"lasttime\":\"1418597513\",\"lat\": 37.17667,
  \"lng\": -122.14650,\"result\":\"ok\"}";

  bool parseSuccess = reader.parse(json, root, false);

  if (parseSuccess)
  {
    const Json::Value resultValue = root["result"];
    cout << "Result is " << resultValue.asString() << "\n";
  }

  Json::StyledWriter styledWriter;
  Json::FastWriter fastWriter;
  Json::Value newValue;
  newValue["result"] = "ok";

  cout << styledWriter.write(newValue) << "\n";
  cout << fastWriter.write(newValue) << "\n";

  return 0;
}
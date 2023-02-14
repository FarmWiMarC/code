#include "esp_http_server.h"
#include "esp_http_client.h"
#include "esp_camera.h"
//#include "camera_index.h"
//#include "camera_index-post.h"
#include <WiFi.h>
#include "Arduino.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"

char buf[50];
int pinOn =4;
int timeperiod=0;
String timefromInternet="--:--:--";
/////////////////////////
String serverName = "ชื่อหรือหมายเลขserver";   // REPLACE WITH  IP ADDRESS

String serverPath = "/wimarc06/uploadCAMV.php";     // The default serverPath should be upload.php

const int serverPort = 80;

WiFiClient client;
boolean WIFIconnected=false;
boolean Flagsystemreset=false;
httpd_handle_t camera_httpd = NULL;
static httpd_handle_t httpServerInstance = NULL;
int bt_timeperiod=0;

int countsleep=0;
char rfID='V';
String lineDevice="wimarc06-CAMV ";

const char *post_dataV = "http://ชื่อหรือหมายเลขserver/wimarc06/InsertdataW32_CAMV.php";//"+ssid+".php"; // insert data in db

////////////////////////

#define relay1 12
#define relay2 13
#define relay3 14
String Outputcontrol = "";
String control ="Pc0";
String timestamp="000000";
String timecheck;
boolean flagsleep = false;
boolean flagpic = true;
float reftime = 0;
int timecount = 0;
int timeend = 0;
String ssids_array[30];
String client_index;
//////////////////////////////////


////////////ble//////////////////////////////////////
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEAdvertisedDevice.h>
#include <BluetoothSerial.h>
#include <BLE2902.h>
boolean flagscan=false;
////////////////////////////////////////////////////////// BLE Define

BLEScan* pBLEScan;


String SSIDBLEs;
String PASSBLEs;

String BLEdata;
String BLEdataout;

bool deviceConnected;
bool flagnotify = false;
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
static void launchSoftAp();
////////////////////////////////////////

////////////splitstring ///////////////////////////
String getValueString(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() + 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

///////////////////////////////////////


void BLEdatacheck(String BLEdatain){
  String dataTmp = BLEdatain;
  String dataCode = dataTmp.substring(0,4);
  Serial.println(dataTmp);



 
 if(dataCode.equals("cams"))
  {
      WIFIconnected=false;
      esp_wifi_stop();
  
   launchSoftAp();
    Serial.println("AP starting....");
httpd_config_t  httpServerConfiguration = HTTPD_DEFAULT_CONFIG();
 httpServerConfiguration.server_port = 80;
Serial.printf("Starting AP web server on port: '%d'\n", httpServerConfiguration.server_port);
static httpd_uri_t testUri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_scan_handler,
   // .handler   = methodHandler,
    .user_ctx  = NULL,
};
static httpd_uri_t postUri = {
    .uri       = "/fill",
    .method    = HTTP_POST,
    .handler   = index_post_handler,
    .user_ctx  = NULL,
}; 

static httpd_uri_t camUri = {
    .uri       = "/cam",
    .method    = HTTP_GET,
    .handler   = stream_handler,
   // .handler   = methodHandler,
    .user_ctx  = NULL,
};
 
    httpServerConfiguration.server_port = 80;
    if(httpd_start(&httpServerInstance, &httpServerConfiguration) == ESP_OK){
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &testUri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &postUri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &camUri));
   // WIFIconnected=true;
  }
  }
  if(dataCode.equals("swon"))
  {
   for (int i=0; i<3;i++)
      { digitalWrite(pinOn,HIGH);
       delay(200); 
       digitalWrite(pinOn,LOW);
       delay(500);
      }
    String switchtime = BLEdatain.substring(5,BLEdatain.length());
    bt_timeperiod = switchtime.toInt(); 
  }

 
  
  if(dataCode.equals("swof"))
  {
   for (int i=0; i<3;i++)
      { digitalWrite(pinOn,HIGH);
       delay(200); 
       digitalWrite(pinOn,LOW);
       delay(500);
      }
    String switchtime = BLEdatain.substring(5,BLEdatain.length());
    timeend = 0; 
  }
  
  
  if(dataCode.equals("sett")){
    for (int i=0; i<3;i++)
      { digitalWrite(pinOn,HIGH);
       delay(200); 
       digitalWrite(pinOn,LOW);
       delay(500);
      }
    String ssid_pass = BLEdatain.substring(5,BLEdatain.length());
    Serial.println(ssid_pass);
    SSIDBLEs=  getValueString(ssid_pass, ':', 0);
    PASSBLEs= getValueString(ssid_pass, ':', 1);
    Serial.print("SSID : ");
    Serial.println(SSIDBLEs);
    Serial.print("PASSWORD : ");
    Serial.println(PASSBLEs);
    BLEdata = "";
    
    wifi_config_t wifi_config = {};
     
     
   strcpy((char*)wifi_config.sta.ssid, SSIDBLEs.c_str());
   strcpy((char*)wifi_config.sta.password,PASSBLEs.c_str());
   Serial.println((char*)wifi_config.sta.password);
   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    //ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
   
    ESP_ERROR_CHECK(esp_wifi_start());
     ESP_LOGI(TAG, "connect to SSID: %s, Pass: %s", wifi_config.sta.ssid, wifi_config.sta.password);


   
     pinMode(pinOn, OUTPUT);
   digitalWrite(pinOn,HIGH);
  delay(500);
 digitalWrite(pinOn,LOW);
  delay(500);
    ESP.restart();
    
    }
  if(dataCode.equals("pass")){
    for (int i=0; i<3;i++)
      { digitalWrite(pinOn,HIGH);
       delay(200); 
       digitalWrite(pinOn,LOW);
       delay(500);
      }
    PASSBLEs = BLEdatain.substring(5,BLEdatain.length());
    Serial.print("PASSWORD : ");
    Serial.println(PASSBLEs);
    BLEdata = "";
    }
  if(dataCode.equals("ssid")){
    for (int i=0; i<2;i++)
      { digitalWrite(pinOn,HIGH);
       delay(200); 
       digitalWrite(pinOn,LOW);
       delay(500);
      }
    SSIDBLEs = BLEdatain.substring(5,BLEdatain.length());
    Serial.print("SSID : ");
    Serial.println(SSIDBLEs);
    BLEdata = "";
    }
  if(dataCode.equals("scan")){
    Serial.println("Reporting...");
    BLEdata = "";
    flagscan = true;
    }
      
  if(dataCode.equals("stat")){
    Serial.print("SSID : ");
    Serial.println(SSIDBLEs);
    Serial.print("PASS : ");
    Serial.println(PASSBLEs);
    Serial.println("Reporting status.");
    Serial.print("IP : ");
    Serial.println(WiFi.localIP());
    Serial.print("Wifi status : ");
    Serial.print(WiFi.status());
    if(WiFi.status()== 0)Serial.println(" WL_IDLE_STATUS");
    else if(WiFi.status()== 1)Serial.println(" WL_NO_SSID_AVAIL");
    else if(WiFi.status()== 2)Serial.println(" WL_SCAN_COMPLETED");
    else if(WiFi.status()== 3)Serial.println(" WL_CONNECTED");
    else if(WiFi.status()== 4)Serial.println(" WL_CONNECT_FAILED");
    else if(WiFi.status()== 5)Serial.println(" WL_CONNECTION_LOST");
    else if(WiFi.status()== 6)Serial.println(" WL_DISCONNECTED");
    else Serial.println(" n/a description.");
    BLEdata = "";
    flagnotify = true;
    } 
    
   if(dataCode.equals("retu")){
    Serial.println("Reporting...");
    BLEdata = "";
    flagnotify = true;
    }

    if(dataCode.equals("rest")){
       delay(1000);
    ESP.restart();
    }
    if(dataCode.equals("conn")){
      
    wifi_config_t wifi_config = {};
     
     
   strcpy((char*)wifi_config.sta.ssid, SSIDBLEs.c_str());
   strcpy((char*)wifi_config.sta.password,PASSBLEs.c_str());
   Serial.println((char*)wifi_config.sta.password);
   wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    //ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
   
    ESP_ERROR_CHECK(esp_wifi_start());
     ESP_LOGI(TAG, "connect to SSID: %s, Pass: %s", wifi_config.sta.ssid, wifi_config.sta.password);



   
    Serial.println(SSIDBLEs.c_str());
    Serial.println(PASSBLEs.c_str());
    //WiFi.mode(WIFI_STA);
    WiFi.begin(SSIDBLEs.c_str(),PASSBLEs.c_str());
     delay(5000);
     Serial.print(WiFi.localIP());
  Serial.println("' to connect");
    Serial.println("Restarting...");
    BLEdata = "";

    storedata(); //Store data from BLE function
    delay(5000);
    ESP.restart();
     pinMode(pinOn, OUTPUT);
   digitalWrite(pinOn,HIGH);
  delay(500);
 digitalWrite(pinOn,LOW);
  delay(500);
    }

}



//////////////////////////////////////BLE function

class MyCallbacks: public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0)
    {
      Serial.println("*********");
      //Serial.print("Value: ");
      for (int i = 0; i < value.length(); i++)
      {
        //Serial.print(value[i]);
        BLEdata.concat(value[i]);
      }
      BLEdataout = BLEdata;
      BLEdatacheck(BLEdataout); 
      Serial.println(BLEdata);
      BLEdata = "";
      //Serial.println();
      Serial.println("*********");
    }
  }
};


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
 
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void storedata(){
 // preferences.putString( "ssid" , SSIDBLEs); 
 // preferences.putString( "password" , PASSBLEs);

  Serial.println("Network Credentials Saved using Preferences");

   
  }
  //////////////////////////////////////////////



String httpdata,httpdatainput;
char* eventdata;
int ind1;
boolean  Flagsleep = false;
boolean Flaggetpic = true;
boolean flagtimer = false;
boolean Flagdatareset=false;




char InputRS232[30];
boolean stringComplete=false;

float tempf,humidf;

int count =0;
int countinternet=0;
int inttemp,inthumid,intCO2;
int v1,v2,v3,v4,v5,v6,v7,v8;

int v1v,v2v,v3v,v4v,v5v,v6v,v7v,v8v;
int v1a,v2a,v3a,v4a,v5a,v6a,v7a,v8a;
int v1b,v2b,v3b,v4b,v5b,v6b,v7b,v8b;
int v1c,v2c,v3c,v4c,v5c,v6c,v7c,v8c;
boolean ConvertData_CheckModeID(int measPoint);

////////////////////////////////////////////////////////////////
String sendPhoto() {
  String getAll;
  String getBody;


  

  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }

  
  Serial.println("Connecting to server: " + serverName);

  if (client.connect(serverName.c_str(), serverPort)) {
    Serial.println("Connection successful!");    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"CAMV.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;
  
    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    client.println();
    client.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0; n<fbLen; n=n+1024) {
      if (n+1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        client.write(fbBuf, remainder);
      }
    }   
    client.print(tail);
    
    esp_camera_fb_return(fb);
    
    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;
    int trycount=0;
    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      delay(100);  
      trycount++;
      if (trycount>30) ESP.restart();   
         
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length()==0) { state=true; }
          getAll = "";
        }
        else if (c != '\r') { getAll += String(c); }
        if (state==true) { getBody += String(c); }
        startTimer = millis();
      }
      if (getBody.length()>0) { break; }
    }
    Serial.println();
    client.stop();
    Serial.println(getBody);
  }
  else {
    getBody = "Connection to " + serverName +  " failed.";
    ESP.restart();
    Serial.println(getBody);
  }
  return getBody;
 
}


/////////////////////////////////////////////////////////////////
static esp_err_t post_sensordata(const char *post_data, int vv1,int vv2, int vv3,int vv4, int vv5, int vv6, int vv7, int vv8);
void serialEvent() {
  int i,j;

   Serial.flush();
   for (i=1;i<10000;i++){
    
    char inchar = Serial.read();
    // add it to the inputString:
     if (inchar == 'P') 
     { InputRS232[0]=inchar;
    for (j=1;j<30;j++){  InputRS232[j] = Serial.read(); }  
     //Serial.println(InputRS232);
     
     
char x=InputRS232[1]+InputRS232[4]+InputRS232[7]+InputRS232[10]+InputRS232[13]+InputRS232[16]+InputRS232[19]+InputRS232[22]+InputRS232[25];
Serial.print("input: ");Serial.println(InputRS232[26]);
Serial.print("x: ");Serial.println(x);
    if ((InputRS232[0] == 'P')&&(InputRS232[1] == 'S'))
    {
      Flagdatareset = false;countsleep =0;
      }
   if ((InputRS232[0] == 'P')&&(InputRS232[26] == x))
  
    {

    
      
      stringComplete=true;
      count = 0;
      
      //readString += c;
     //i=20000;
     //Serial.println(InputRS232);
     if (ConvertData_CheckModeID(0))
     {InputRS232[0] == 'Q';
     
    if (v7>0)
    {
       
      Serial.print(Outputcontrol);
      Serial.print(InputRS232[1]);Serial.print(':');  
      Serial.print(v1);Serial.print(':');
      Serial.print(v2);Serial.print(':');
      Serial.print(v3);Serial.print(':');
      Serial.print(v4);Serial.print(':');
      Serial.print(v5);Serial.print(':');
      Serial.print(v6);Serial.print(':');
      Serial.print(v7);Serial.print(':');
      Serial.print(v8);Serial.print(':');
      stringComplete=true;
     switch (InputRS232[1]) {
     case 'V':
       
       v1v=v1;
       v2v=v2;
       v3v=v3;
       v4v=v4;
       v5v=v5;
       v6v=v6;
       v7v=v7;
       v8v=v8;
        post_sensordata(post_dataV,v1v,v2v,v3v,v4v,v5v,v6v,v7v,v8v);
        if (flagtimer)       
        post_sensordata(post_datacontrol,v1v,v2v,v3v,v4v,v5v,v6v,timecount,v8v);
        break;
     case 'A':
       client_index = "client A";
       v1a=v1;
       v2a=v2;
       v3a=v3;
       v4a=v4;
       v5a=v5;
       v6a=v6;
       v7a=v7;
       v8a=v8; 
        post_sensordata(post_dataA,v1a,v2a,v3a,v4a,v5a,v6a,v7a,v8a);
        break;
     case 'B':
        client_index = "client B";
        v1b=v1;
       v2b=v2;
       v3b=v3;
       v4b=v4;
       v5b=v5;
       v6b=v6;
       v7b=v7;
       v8b=v8;
        post_sensordata(post_dataB,v1b,v2b,v3b,v4b,v5b,v6b,v7b,v8b);
        break;
      case 'C':
        client_index = "client C";
        v1c=v1;
       v2c=v2;
       v3c=v3;
       v4c=v4;
       v5c=v5;
       v6c=v6;
       v7c=v7;
       v8c=v8;
        post_sensordata(post_dataC,v1c,v2c,v3c,v4c,v5c,v6c,v7c,v8c);
        break;
     }      
       
      delay(500);
      break;
      } 
     else
     {
      Serial.println("Data Error");
        
      }
     }//if (ConvertData_CheckModeID(0))
   
      
    // while (Serial2.available()){}
    }
    }
  //Serial.println(inputString);
  delay(1);
  if (i%1000==0) 
  {Serial.println(Outputcontrol);Serial.println(i);}
  }
  for (j=1;j<3;j++){  InputRS232[j] = '0'; }   
}

//////////////////////////////////////////////////////////
boolean ConvertData_CheckModeID(int measPoint)
{char i,data[3],checkchar;
 boolean FlagSearch;
 int tmp;
 char  Data1,Data2,Data3,Data4,Data5;
 int tmp1,tmp2;

 
 FlagSearch =1;i=0;//k=0;

//char x=InputRS232[1]+InputRS232[4]+InputRS232[7]+InputRS232[10]+InputRS232[13]+InputRS232[16]+InputRS232[19]+InputRS232[22]+InputRS232[25]+InputRS232[28]+InputRS232[31];
char x=InputRS232[1]+InputRS232[4]+InputRS232[7]+InputRS232[10]+InputRS232[13]+InputRS232[16]+InputRS232[19]+InputRS232[22]+InputRS232[25];

   if (InputRS232[32] != x)    {
//    char x =InputRS232[5]+InputRS232[8]+InputRS232[1]+InputRS232[2];
    Serial.print("Input ="); Serial.println(InputRS232[26]);
    Serial.print("check ="); Serial.println(x);
    delay(1000);
    
  //  return false;  
  } 


 /////////////////v1  
  i=InputRS232[1];
  Data1=InputRS232[2];
  Data2=InputRS232[3];
  Data3=InputRS232[4];

  checkchar=i+Data1+Data2;
 if (Data3 == checkchar )
    {  v1 =0;
  tmp =Data1;
  tmp <<= 8;
  tmp &= 0xff00;//
  v1=Data2;
 
  v1 &= 0x00ff;
  v1 |= tmp;   
 
//v1=InputRS232[3]+InputRS232[4];
  v1-=1000;
 

}

 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { return false;}



 



/////////////////v2  
   Data1=InputRS232[5];
   Data2=InputRS232[6];
   Data3=InputRS232[7];

  checkchar=i+Data1+Data2;
 if (Data3 == checkchar )
 {
  v2 =0;
  tmp = Data1;
  tmp <<= 8;
  tmp &= 0xff00;//
  v2=Data2;
 
  v2 &= 0x00ff;
  v2 |= tmp;   
 
 v2-=1000;

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { return false;}

 
/////////////////v3  
    Data1=InputRS232[8];
    Data2=InputRS232[9];
    Data3=InputRS232[10];
   checkchar=i+Data1+Data2;
 if (Data3 == checkchar )
 {v3 =0;
  tmp =  Data1;
  tmp <<= 8;
  tmp &= 0xff00;//
  v3= Data2;
 
  v3 &= 0x00ff;
  v3 |= tmp;   
v3-=1000;

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { return false;}

/////////////////v4  
    Data1=InputRS232[11];
    Data2=InputRS232[12];
    Data3=InputRS232[13];
   checkchar=i+Data1+Data2;
 if (Data3 == checkchar )
 {v4 =0;
  tmp =  Data1;
  tmp <<= 8;
  tmp &= 0xff00;//
  v4= Data2;
 
  v4 &= 0x00ff;
  v4 |= tmp;   

 v4-=1000;



 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { return false;}


/////////////////v5  
    Data1=InputRS232[14];
    Data2=InputRS232[15];
    Data3=InputRS232[16];
    checkchar=i+Data1+Data2;
 if (Data3 == checkchar )
 {v5 =0;
  tmp =  Data1;
  tmp <<= 8;
  tmp &= 0xff00;//
  v5= Data2;
 
  v5 &= 0x00ff;
  v5 |= tmp;   
 v5-=1000;

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { return false;}

/////////////////v6  
    Data1=InputRS232[17];
    Data2=InputRS232[18];
    Data3=InputRS232[19];
    checkchar=i+Data1+Data2;
 if (Data3 == checkchar )
 {v6 =0;
  tmp =  Data1;
  tmp <<= 8;
  tmp &= 0xff00;//
  v6= Data2;
 
  v6 &= 0x00ff;
  v6 |= tmp;   
 v6-=1000;

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { return false;}



/////////////////v7  
    Data1=InputRS232[20];
    Data2=InputRS232[21];
    Data3=InputRS232[22];
 checkchar=i+Data1+Data2;
 if (Data3 == checkchar )
 {v7 =0;
  tmp =  Data1;
  tmp <<= 8;
  tmp &= 0xff00;//
  v7= Data2;
 
  v7 &= 0x00ff;
  v7 |= tmp;   
 v7-=1000;

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { return false;}

/////////////////v8  
    Data1=InputRS232[23];
    Data2=InputRS232[24];
    Data3=InputRS232[25];
checkchar=i+Data1+Data2;
 if (Data3 == checkchar )
 {v8 =0;
  tmp =  Data1;
  tmp <<= 8;
  tmp &= 0xff00;//
  v8= Data2;
 
  v8 &= 0x00ff;
  v8 |= tmp;   
 v8-=1000;

 }


return true;

 
}

///////////////////////////////////

// Replace with your network credentials
#include <nvs.h>
#include <nvs_flash.h>

 
#define SOFT_AP_IP_ADDRESS_1 10
#define SOFT_AP_IP_ADDRESS_2 10
#define SOFT_AP_IP_ADDRESS_3 10
#define SOFT_AP_IP_ADDRESS_4 1
 
#define SOFT_AP_GW_ADDRESS_1 10
#define SOFT_AP_GW_ADDRESS_2 10
#define SOFT_AP_GW_ADDRESS_3 10
#define SOFT_AP_GW_ADDRESS_4 20
 
#define SOFT_AP_NM_ADDRESS_1 255
#define SOFT_AP_NM_ADDRESS_2 255
#define SOFT_AP_NM_ADDRESS_3 255
#define SOFT_AP_NM_ADDRESS_4 0
 
#define SERVER_PORT 3500
#define HTTP_METHOD HTTP_POST
#define URI_STRING "/test"



// Variable to store the HTTP request
String header;


String page;
const char HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('ssid').value=l.innerText||l.textContent;document.getElementById('pass').focus();}</script>";
const char HTTP_ITEM[]  = "<div><a href='#' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
int           _minimumQuality         = 0;
boolean flagconnect=true;
//const char* ssid = "Goyangle";
//const char* password = "pi3thnop";
//char* ssid = "";
//char* password = "";
int capture_interval = 5000; // Microseconds between captures
String url;
 
bool internet_connected = false;
long current_millis;
long last_capture_millis = 0;
long last_feed_millis = 0;
// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
void APloop();

boolean init_wifi();

//void startCameraServer();
int getRSSIasQuality(int RSSI) {
  int quality = 0;

  if (RSSI <= -100) {
    quality = 0;
  } else if (RSSI >= -50) {
    quality = 100;
  } else {
    quality = 2 * (RSSI + 100);
  }
  Serial.print (RSSI);Serial.print (":");Serial.print (quality);
  return quality;
}

esp_err_t index_post_handler(httpd_req_t *req)
{
wifi_config_t wifi_config = {};
     
     uint8_t buffer[2048];
   // char buffer[128];
   strcpy((char*)wifi_config.sta.ssid, "");
   strcpy((char*)wifi_config.sta.password,"");
   (void) memset(buffer, 0, sizeof(buffer));
    httpd_req_recv(req, (char *) buffer,sizeof(buffer));
    int len;
    Serial.println("return:");
    Serial.println((char*)buffer);
    if (httpd_query_key_value((char *) buffer, "ssid", (char *) wifi_config.sta.ssid, 20) == ESP_ERR_NOT_FOUND) {
    //if (httpd_query_key_value((char *) buffer, "ssid", ssid, 32) == ESP_ERR_NOT_FOUND) {
    
        httpd_resp_set_status(req, "400");
        httpd_resp_send(req, "SSID required", -1);
        return ESP_OK;
    }
    
    if (httpd_query_key_value((char *) buffer, "password", (char *) wifi_config.sta.password, 20) == ESP_ERR_NOT_FOUND) {
    //if (httpd_query_key_value((char *) buffer, "password", password, 20) == ESP_ERR_NOT_FOUND) {

        httpd_resp_set_status(req, "400");
        httpd_resp_send(req, "Password is required", -1);
        return ESP_OK;
    }
 
    
 
    if (strlen((char *) wifi_config.sta.ssid) < 1) {
        
        httpd_resp_set_status(req, "400");
        httpd_resp_send(req, "<p>Invalid ssid</p>", -1);
        return ESP_OK;
    }
 
    ESP_LOGI(TAG, "SSID: %s, Pass: %s", wifi_config.sta.ssid, wifi_config.sta.password);
    httpd_resp_send(req, "<h1>OK</h1>", -1);
    Serial.println(strlen((char *) wifi_config.sta.password));
     int connAttempts=0;
    //strcpy((char*)wifi_config.sta.ssid, (const char*)ssid);
    //strcpy((char*)wifi_config.sta.password, (const char*)password); 
    //ssid=(char*)wifi_config.sta.ssid;
    //password=(char*) wifi_config.sta.password;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//strcpy((char*)wifi_config.sta.ssid, (const char*)ssid);
//strcpy((char*)wifi_config.sta.password, (const char*)password);
 
//strcpy((char*)ssid),(const char*)wifi_config.sta.ssid;
//strcpy((char*)password,(const char*)wifi_config.sta.password);
 //  strcpy((char*)wifi_config.sta.ssid, ssid);
 //  strcpy((char*)wifi_config.sta.password,password);
    
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
   
   // ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg) );
    ESP_ERROR_CHECK(esp_wifi_start());

      
  
  ESP_LOGI(TAG, "connect to SSID: %s, Pass: %s", wifi_config.sta.ssid, wifi_config.sta.password);

 

   flagconnect=true; 
  // WiFi.mode(WIFI_STA);
   WiFi.begin();
 int wificount=0;
  while (WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
   wificount = wificount+1;
   if (wificount > 50)
   {pinMode(relay1, OUTPUT);
     digitalWrite(relay1,LOW); 
   }
 }
  
    
  
      
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 5000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
   // config.frame_size = FRAMESIZE_UXGA;
    config.frame_size = FRAMESIZE_XGA;
    config.jpeg_quality = 15;
    config.fb_count = 10;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 15;
    config.fb_count = 10;
  }
//startCameraServer();
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
   // return;
  }
   sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
  s->set_framesize(s, FRAMESIZE_SXGA);
  //s->set_jpeg_quality(s, 12);

WIFIconnected=true;



  

  
    
    return ESP_OK;
}


static esp_err_t index_scan_handler(httpd_req_t *req){

   
   const char * resp_str = "<h1>Choose access point</h1>";//<form method=\"POST\" action=\"http://192.168.1.9/\">";
   int n = WiFi.scanNetworks();
   Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    };
   
    Serial.println(""); 
     int indices[n];

      for (int i = 0; i < n; i++) {
        indices[i] = i;
      }
      page="";
   //display networks in page
      for (int i = 0; i < n; i++) {
       
        int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));
         
        //if (_minimumQuality == -1 || _minimumQuality < quality) {
         
           if ( _minimumQuality < quality) {
            
          String item = FPSTR(HTTP_ITEM);
          String rssiQ;
          rssiQ += quality;  
        //  Serial.println(WiFi.SSID(indices[i]));
          
          item.replace("{v}", WiFi.SSID(indices[i]));
          item.replace("{r}", rssiQ);
          /*
          if (WiFi.encryptionType(indices[i]) != WIFI_AUTH_OPEN) {
            item.replace("{i}", "l");
          } else {
            item.replace("{i}", "");
          }
          //DEBUG_WM(item);
          */
          page += item;
        //  Serial.println(item);
          delay(0);
        } else {
          Serial.println("Skipping due to quality");
        }

      }
      page += "<br/>";

  String html= "<!DOCTYPE HTML><html><head><meta name = \"viewport\" http-equiv=\"content-type\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\"><title>WIMARC32</title>";
  String style= "<style>body { background-color: #0067B3  ; font-family: Arial, Helvetica, Sans-Serif; Color: #FFFFFF; }</style></head>";
  String headStyle= "<body><center><h1 style=\"color:#FFFFFF; font-family:verdana;font-family: verdana;padding-top: 10px;padding-bottom: 10px;font-size: 36px\">WIMARC32cam</h1><h2 style=\"color:#FFFFFF;font-family: Verdana;font: caption;font-size: 27px;padding-top: 10px;padding-bottom: 10px;\">Give Your WiFi Credentials</h2>";


   
   String resp = html+style+headStyle+String(resp_str)+page;
       String s = "   <form method='POST' action=\"/fill\">\r\n";   
            s += "   <label>SSID: </label> \r\n";   
           s += "   <input   style=\"font-size:25px\" id='ssid' name='ssid' length=20> \r\n";   
           s += "    <label>Password: </label> \r\n";   
           s += "   <input style=\"font-size:25px\"  id = 'password' name='password' length=20> \r\n";  
           s += "  <input style=\"font-size:25px\"  id = 'id' name='id' length=0> \r\n"; 
           s += " <br>  <input style=\"font-size:40px; width: 200px; height: 50px;\"  type='submit'></br></form> \r\n";   

   
   String script=FPSTR(HTTP_SCRIPT);
   resp=script+resp+s;    // to use the concatenation in const char* use
   resp_str=   resp.c_str();
   //Serial.println(resp_str);
    return    httpd_resp_send(req, resp_str, strlen(resp_str));
   
    }

///////////////////////////////////////////////
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;

static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    return res;
  }

  while(true){
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if(fb->width > 400){
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if(res != ESP_OK){
      break;
    }
    //Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
  }
  return res;
}




String checkSSID()
{
   wifi_config_t conf;
 esp_wifi_get_config(WIFI_IF_STA, &conf);
 return String(reinterpret_cast<const char*>(conf.sta.ssid));
 
}
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  
////////////////////////////////////
    pinMode(relay1, OUTPUT);
   
   digitalWrite(relay1,LOW);

    pinMode(relay2, OUTPUT);
   
  digitalWrite(relay2,LOW);

 // pinMode(13, INPUT);
 //  pinMode(13, INPUT_PULLUP);
 // attachInterrupt(13, isr, FALLING);

  
 Outputcontrol = "PT"+timestamp+control;
 
   Serial.begin(9600);
  Serial.setDebugOutput(true);
 //Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); 


 //url+=ssid;
 //url+="CAM1.php";

while (Serial.available()) 
     Serial.read();
 delay(2000);
 Serial.println(lineDevice);
 ////////////////////////////////////////////////////////

/////////////////////////////////////////BLE test v1

  Serial.println("BLE initailizing..");

  BLEDevice::init(lineDevice.c_str());
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
 
  BLEService *pService = pServer->createService(SERVICE_UUID);
 
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
 
  pCharacteristic->addDescriptor(new BLE2902());
 
  pService->start();
 
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

  pCharacteristic->setCallbacks(new MyCallbacks());
   String countstring=timefromInternet;
 pCharacteristic->setValue(countstring.c_str());
 
  pService->start();

  //BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  Serial.println("BLE start successful!");
  Serial.print("Free heap : ");
  Serial.println(ESP.getFreeHeap());
////////////////////////////////////////////////////////////////////////



int n = WiFi.scanNetworks();
   Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    };


  
  
 
 if (init_wifi(WIFIconnected)) { // Connected to WiFi
    internet_connected = true;
    Serial.println("Internet connected");  
    Serial.println(checkSSID()); 
 

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.print(WiFi.localIP());
 
  Serial.println("' to connect");

 /////////////////
 
//////////////////

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 5000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_XGA;
    // config.frame_size = FRAMESIZE_SXGA;
    //config.jpeg_quality = 5;
    //config.fb_count = 10;
     //config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 5; //10-63 lower number means higher quality
    config.fb_count = 1;
  } else {
    //config.frame_size = FRAMESIZE_SVGA;
    config.frame_size = FRAMESIZE_VGA;
    
    config.jpeg_quality = 12;
    config.fb_count = 10;
  }
//startCameraServer();
  // camera init
  esp_err_t err = esp_camera_init(&config);

    
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  //Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
 
   sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
  s->set_framesize(s, FRAMESIZE_XGA);
 // s->set_framesize(s, FRAMESIZE_UXGA);

 
WIFIconnected=true;
post_sensordata(post_datacontrol,0,0,0,0,0,0,99999,0);    
//startCameraServer();
  }
   
 else
 {
  
  launchSoftAp();
 Serial.println("AP starting....");
httpd_config_t  httpServerConfiguration = HTTPD_DEFAULT_CONFIG();
 httpServerConfiguration.server_port = 80;
Serial.printf("Starting AP web server on port: '%d'\n", httpServerConfiguration.server_port);
static httpd_uri_t testUri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_scan_handler,
   // .handler   = methodHandler,
    .user_ctx  = NULL,
};
static httpd_uri_t postUri = {
    .uri       = "/fill",
    .method    = HTTP_POST,
    .handler   = index_post_handler,
    .user_ctx  = NULL,
}; 

static httpd_uri_t camUri = {
    .uri       = "/cam",
    .method    = HTTP_GET,
    .handler   = stream_handler,
   // .handler   = methodHandler,
    .user_ctx  = NULL,
};
 
    httpServerConfiguration.server_port = 80;
    if(httpd_start(&httpServerInstance, &httpServerConfiguration) == ESP_OK){
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &testUri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &postUri));
        ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &camUri));
   // WIFIconnected=true;
    }

if (WIFIconnected)

return;

 
 }
} 
////////////////////


 
 static httpd_uri_t testUri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_scan_handler,
    .user_ctx  = NULL,
};


static void launchSoftAp(){
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
    tcpip_adapter_ip_info_t ipAddressInfo;
    memset(&ipAddressInfo, 0, sizeof(ipAddressInfo));
    IP4_ADDR(
        &ipAddressInfo.ip,
        SOFT_AP_IP_ADDRESS_1,
        SOFT_AP_IP_ADDRESS_2,
        SOFT_AP_IP_ADDRESS_3,
        SOFT_AP_IP_ADDRESS_4);
    IP4_ADDR(
        &ipAddressInfo.gw,
        SOFT_AP_GW_ADDRESS_1,
        SOFT_AP_GW_ADDRESS_2,
        SOFT_AP_GW_ADDRESS_3,
        SOFT_AP_GW_ADDRESS_4);
    IP4_ADDR(
        &ipAddressInfo.netmask,
        SOFT_AP_NM_ADDRESS_1,
        SOFT_AP_NM_ADDRESS_2,
        SOFT_AP_NM_ADDRESS_3,
        SOFT_AP_NM_ADDRESS_4);
    ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ipAddressInfo));
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
   // ESP_ERROR_CHECK(esp_event_loop_init(wifiEventHandler, NULL));
    wifi_init_config_t wifiConfiguration = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifiConfiguration));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t apConfiguration = {};
    strcpy((char*)apConfiguration.ap.ssid, "wimarc32CAM");
    strcpy((char*)apConfiguration.ap.password, "");
     apConfiguration.ap.ssid_len=strlen("wimarc32CAM");
   //apConfiguration.ap.authmode="WIFI_AUTH_OPEN";
   // strcpy((char*)apConfiguration.ap.ssid_hidden, "0");
   apConfiguration.ap.max_connection=1;
   // strcpy((char*)apConfiguration.ap.beacon_interval, "150");
    /*
        .ap = {
            .ssid = SOFT_AP_SSID,
            .password = SOFT_AP_PASSWORD,
            .ssid_len = 0,
            //.channel = default,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = 0,
            .max_connection = 1,
            .beacon_interval = 150,
        },
    };
    */ ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        ESP_LOGI(TAG, "WiFi %s ", apConfiguration.ap.ssid);
        ESP_LOGI(TAG, "PSW %s ", apConfiguration.ap.password);
       
    
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &apConfiguration));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_start());
}


//////////
bool init_wifi(bool wificonnected)
{
  int connAttempts = 0;
  int limit =100;
  if (wificonnected) limit =200;
 
  // WiFi.mode(WIFI_STA);
   WiFi.begin();
  //WiFi.mode(WIFI_STA);

  // pinMode(12, INPUT); 
  // int val=0;

   pinMode(relay1, INPUT); 
   pinMode(pinOn, OUTPUT);
   //digitalWrite(pinOn,HIGH);
     
    
   int i=0;
  while (WiFi.status() != WL_CONNECTED ) {
    // val = digitalRead(12);
    // if(val ==1)
    // return false;// read the input pin
 
    i++;
   /* Serial.print(".");

   if (connAttempts>3) digitalWrite(pinOn,LOW);
         else
         { 
       digitalWrite(pinOn, !digitalRead(pinOn));
         } 
      
   */  
   
    
    if  (digitalRead(relay1)== HIGH)
       {
       pinMode(pinOn, OUTPUT);
       digitalWrite(pinOn,HIGH);
       delay(500); 
       digitalWrite(pinOn,LOW);
       delay(500);
       digitalWrite(pinOn,HIGH);
       delay(500); 
       digitalWrite(pinOn,LOW);
       pinMode(relay1, OUTPUT);
       digitalWrite(relay1,HIGH);
       return false;
       }
        
    if (connAttempts > limit)  
    {
      pinMode(relay1, OUTPUT);
      digitalWrite(relay1,HIGH); 
      return false;
    }
    connAttempts++;
   delay(500);
    Serial.println(connAttempts);
    delay(500);
    
  }
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1,LOW);
  pinMode(pinOn, OUTPUT);
   digitalWrite(pinOn,HIGH);
  delay(500);
 digitalWrite(pinOn,LOW);
  delay(500);
 
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  return true;
  
}

//////////////////////////////////////////////////////////////
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
  switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      Serial.println(Outputcontrol);
      Serial.print("HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      Serial.println(Outputcontrol);
      Serial.print("HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      Serial.println(Outputcontrol);
      Serial.print("HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      Serial.println();
      Serial.println(Outputcontrol);
      //Serial.printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      httpdatainput = evt->header_value;
      ind1 = httpdatainput.indexOf(":");  //finds location of first ,
      //Serial.print("index:");Serial.println(ind1);
      if (ind1 >0)
      {httpdata = httpdatainput.substring(ind1+1, ind1+3);
      //Serial.print(httpdata);
      }
      break;
    case HTTP_EVENT_ON_DATA:
      Serial.println();
      Serial.print(Outputcontrol);
      Serial.printf("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      eventdata = (char*)evt->data;
      Serial.println(eventdata);
      if (!esp_http_client_is_chunked_response(evt->client)) {
       //  Write out data
        // printf("%.*s", evt->data_len, (char*)evt->data);
       eventdata = (char*)evt->data;
 
      
      }
      break;
    case HTTP_EVENT_ON_FINISH:
      Serial.println(Outputcontrol);
      Serial.print("HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      Serial.println(Outputcontrol);
      Serial.print("HTTP_EVENT_DISCONNECTED");
      break;
  }
  return ESP_OK;
}

//##########################################################################################################################
static esp_err_t post_sensordata(const char *post_data, int vv1,int vv2, int vv3,int vv4, int vv5, int vv6, int vv7, int vv8)
{
   String Input ="&A="+String(vv1)+"&B=";
 Input += String(vv2)+"&C=";
 Input += String(vv3)+"&D=";
  Input += String(vv4)+"&E=";
   Input += String(vv5)+"&F=";
    Input += String(vv6)+"&G=";
     Input += String(vv7)+"&H=";
      Input += String(vv8);
   




  
  Serial.println("POST sensor data...");
  esp_err_t res = ESP_OK;

  esp_http_client_handle_t http_client;
  
  esp_http_client_config_t config_client = {0};
  config_client.url = post_data;
  config_client.event_handler = _http_event_handler;
  config_client.method = HTTP_METHOD_POST;

  http_client = esp_http_client_init(&config_client);
 
 

     
 Input= "date=1&time=2"+Input;
      

 Serial.println(Input);
 
 //Input.toCharArray(InputRS232,30);
  esp_http_client_set_post_field(http_client,(const char *) Input.c_str(), strlen(Input.c_str()));

  // esp_http_client_set_header(http_client, "Content-Type", "image/jpg"); // sending a jpg file

  esp_err_t err = esp_http_client_perform(http_client);
  if (err == ESP_OK) {
    //Serial.print("esp_http_client_get_status_code: ");
    //Serial.println(esp_http_client_get_status_code(http_client));
   // Serial.println(eventdata);
    
   char * item = strtok (eventdata, ":"); //getting first word (uses space & comma as delimeter)
    
    
  }

  esp_http_client_cleanup(http_client);
}



//###################################################################
static esp_err_t post_time()
{

  Serial.println("POST time...");
  esp_err_t res = ESP_OK;

  esp_http_client_handle_t http_client;

  esp_http_client_config_t config_client = {0};
  
  config_client.url = post_gettime;
  config_client.event_handler = _http_event_handler;
  config_client.method = HTTP_METHOD_GET;

  http_client = esp_http_client_init(&config_client);




  String Input = "date=1&time=2";


  //Serial.println(Input);

  //Input.toCharArray(InputRS232,30);
  esp_http_client_set_post_field(http_client, (const char *) Input.c_str(), strlen(Input.c_str()));

  // esp_http_client_set_header(http_client, "Content-Type", "image/jpg"); // sending a jpg file

  esp_err_t err = esp_http_client_perform(http_client);
  if (err == ESP_OK) {
    Serial.print(Outputcontrol);
    Serial.println(eventdata);

    // Serial.println(esp_http_client_get_status_code(http_client));
    char * item = strtok (eventdata, ","); //getting first word (uses space & comma as delimeter)

    timecheck = (String) item;
    Serial.println(Outputcontrol);
    Serial.print("timecheck:"); Serial.println(timecheck);
    timefromInternet = timecheck;
    ind1 = timecheck.indexOf(":");  //finds location of first ,

    if (ind1 > 0)
    { httpdata = timecheck.substring(ind1 + 1, ind1 + 3);
      
      String hourSt = timecheck.substring(ind1 -2, ind1 );
      String minSt = timecheck.substring(ind1 +1, ind1 +3);
      String secSt = timecheck.substring(ind1 +4, ind1 +6);
     // Serial.print("ind1");Serial.print(ind1);Serial.print("---TIME:");Serial.println(hourSt+minSt+secSt);
      timestamp = hourSt+minSt+secSt;
      //Outputcontrol = "PT"+timestamp+control;

      
      int timeminute = (httpdata.toInt() % 10) * 1;




      if (timeminute == 0)

      {
        flagsleep = true;

      }
      if (timeminute > 7)

      {
        flagpic = false;
      }
      else
      {
        flagpic = true;
      }
      Serial.println(Outputcontrol);
      Serial.print("httpdata:"); Serial.println(timeminute);


     // item = strtok (NULL, ","); //getting subsequence word
       item = strtok (NULL, "@"); //getting subsequence word
      Serial.println(Outputcontrol);
      Serial.print("item:"); Serial.println(item);
      timecheck = (String) item;
      int n = timecheck.length();
      if (n == 16)
      { 
    // declaring character array
    
    char char_array[n + 1];
    
    // copying the contents of the
    // string to char array
    strcpy(char_array, timecheck.c_str());
     Serial.print("controldatalength:");Serial.println(n);
    for (int i = 0; i < n; i++)
    {  Serial.print(char_array[i],HEX); Serial.print(':'); 
    }
  int Data1,Data2,Data3;
  Data1=char_array[0];
  Data2=char_array[1];
  Data3=char_array[2];
 
 char checkchar=Data1+Data2;
 int tmp;
 if (Data3 == checkchar )
    {  timeperiod =0;
       tmp =Data1;
       tmp <<= 8;
       tmp &= 0xff00;//
       timeperiod=Data2;
 
       timeperiod &= 0x00ff;
       timeperiod |= tmp;   
 
      //v1=InputRS232[3]+InputRS232[4];
       timeperiod-=1000;
    }
   else 
    timeperiod =0;  
  Serial.print("timeperiod:");Serial.println(timeperiod);
  //Serial.print("eventdata:");Serial.println(eventdata);
    if (timeperiod == 10000) 
    
    { 
      
      Serial.println("...................................->STOP");
      timeperiod =0;
      timeend =0;
      pinMode(relay2, OUTPUT);
      digitalWrite(relay2, HIGH);
      for (int i=0; i<3;i++)
      { digitalWrite(pinOn,HIGH);
       delay(200); 
       digitalWrite(pinOn,LOW);
       delay(500);
      }
      ESP.restart();
    }
    else  
    
    timeperiod = timeperiod * 60;

if (timeperiod >0) 
control="Pc1"+timecheck;
else 
control="Pc0"+timecheck;
   
      
      }
    
     
    }
    else
    {
      Serial.println("httpdata:ERROR");
      while (true)
      {
        Serial.println("PX00ERRORtime");
        delay(500);
        
        }
    }

  countinternet=0;
  }
  else 
    {countinternet++;
    if (countinternet >30)
    while(true)
    {
     Serial.println("PX10101005");delay(1000); 
     }
    
    };

  esp_http_client_cleanup(http_client);
}


int input13=0;
void loop()
{
//////////////////////////////////////////////

if (deviceConnected ||(stringComplete)){
 String countstring=timefromInternet+"/count:"+String(count)+'/'+client_index;
 if (flagtimer) 
  countstring = timefromInternet+"/count:"+String(count)+"/time:"+String(timeend)+"ON:"+String(timecount);
 pCharacteristic->setValue(countstring.c_str());  
 pCharacteristic->notify();
 stringComplete=false;
   
}


if (deviceConnected && (flagnotify))  {
    String ssidsend = "SSID:" + checkSSID();
    WiFi.localIP().toString().c_str();
     String ipsend=WiFi.localIP().toString();
     String allsend = ssidsend + "/" + ipsend;
      
        pCharacteristic->setValue(allsend.c_str());
        pCharacteristic->notify();
        flagnotify = false;
               
        
    }


/////////////////////////////////////////////
count=count+1;
 //Serial.println(Outputcontrol);
 Serial.print(" count: ");Serial.print(count);
 if (count > 30)
     {
      for(int x=0;x<30;x++)
      {
      Serial.print("PX00count");Serial.println(count);
      delay(500);
      
      }//pinMode(relay1, OUTPUT);
     pinMode(relay2, OUTPUT);
  digitalWrite(relay2,HIGH);
     
     
     //digitalWrite(relay1,HIGH); 
     }
 
//serialEvent();


  
if (WIFIconnected)  
{
    serialEvent();
    if (WiFi.status() != WL_CONNECTED) {
    delay(1);  init_wifi(WIFIconnected);
    return;
    }

    //post_time();
    current_millis = millis();
    
    if( Flagdatareset )
    {
       for (int i=0;i<20;i++)
       {
       Serial.print("PS");Serial.print(WiFi.localIP());Serial.print(',');Serial.print(countsleep);Serial.println();
       delay(500);
       }
     countsleep = countsleep+1;
     
     if (countsleep > 30)
     {pinMode(relay1, OUTPUT);
     digitalWrite(relay1,LOW); 
     Flagsystemreset=true;
     
     
     }
     Serial.print("countsleep: ");Serial.println(countsleep);

   //  Flagshutdown =false;
       
    }
       
     if (flagtimer == false)
      {
        
        if ((current_millis - last_capture_millis > capture_interval)) 
        { // Take another picture
  
         last_capture_millis = millis();
    
        sendPhoto(); 
        //take_send_photo();
        //delay(5000);
          
      
      
        }
      
        
        post_time();
    
    
    /////////check bluetooth setting///////////////
    
    if (bt_timeperiod > 0)
    {
      timeperiod = bt_timeperiod;
    }
    //timeperiod = timecheck.toInt();
    
    
    Serial.print("Timeperiod:"); Serial.println(timeperiod);
      

    if ((timeperiod == 0) )
       {
      pinMode(relay1, OUTPUT);
      digitalWrite(relay1, LOW);
      
      Outputcontrol = "PT"+timestamp+control;
      if (Flagdatareset )
      Outputcontrol = "PS"+timestamp+control;
      if (Flagsystemreset)  Outputcontrol = "PX"+timestamp+control;     
      }

     
    else
    { 
      
      
      
      reftime = millis();
      flagtimer = true;
      timecount = 0;
      timeend = timeperiod;
      //timeend= 60;
      input13=0;
      
      //post_sensordata(post_dataControl,0,0,0,0,0,0,timecount,timeend);

      
      pinMode(pinOn, OUTPUT);
      digitalWrite(pinOn,HIGH);
      delay(500);
      pinMode(pinOn, OUTPUT);
      digitalWrite(pinOn, LOW);
      pinMode(relay1, OUTPUT);
      digitalWrite(relay1, HIGH);
    
    
      
      
     Outputcontrol = "PT"+timestamp+control;
     if (Flagdatareset)  Outputcontrol = "PS"+timestamp+control;         
            
    }

   Serial.print("eventdata:");Serial.println(eventdata);
    int timeminute=httpdata.toInt()%10;
    if (timeminute == 0)
    {
    Flagdatareset = true; 
    post_sensordata(post_dataV,v1v,v2v,v3v,v4v,v5v,v6v,v7v,v8v);
    
    delay(1000);
    post_sensordata(post_dataA,v1a,v2a,v3a,v4a,v5a,v6a,v7a,v8a);
    delay(1000);
    post_sensordata(post_dataB,v1b,v2b,v3b,v4b,v5b,v6b,v7b,v8b);
    delay(1000);
    post_sensordata(post_dataC,v1c,v2c,v3c,v4c,v5c,v6c,v7c,v8c);
    delay(1000);
    v1v=v2v=v3v=v4v=v5v=v6v=v7v=v8v=0;
    v1a=v2a=v3a=v4a=v5a=v6a=v7a=v8a=0;
    v1b=v2b=v3b=v4b=v5b=v6b=v7b=v8b=0;
    v1c=v2c=v3c=v4c=v5c=v6c=v7c=v8c=0;
    
    }

        
      }
    else  //flagtimer
      { 
         if ((current_millis - last_capture_millis > capture_interval)) 
        { // Take another picture
  
         last_capture_millis = millis();
    
     
       // take_send_photo();
        //delay(5000);
         sendPhoto(); 
      
      
        }
      
        
        post_time();
        
        
        timecount = (int)((millis() - reftime) / 1000);
        Serial.print("..........timecount:"); Serial.print(timecount);Serial.print("/");Serial.println(timeend);
        if ((timecount/30) == 0 ) 
        {
        Serial.println("--------------->send LINE");
      
        
        sendPhoto();
        delay(500);
        
        }
        
        ///////////////
       
       ////////////////////////////
     
       
      if ((timecount > timeend))
      { //flagsleep = false;
        flagtimer = false;
        timecount = 0;
        timeperiod = 0;
        timeend=0;
        input13=0;
        bt_timeperiod=0;
        // pinMode(pinOn, OUTPUT);
        // digitalWrite(pinOn,HIGH);
        delay(500);
        pinMode(relay1, OUTPUT);
        delay(1000);
        digitalWrite(relay1, LOW);
       
        sendPhoto();
        //take_send_photo();

        
        
        control ="Pc0";
        Outputcontrol ="PT"+timestamp+control;
        //post_sensordata(post_datacontrol,v1v,v2v,v3v,v4v,v5v,v6v,timecount,v8v);
        
        if (Flagdatareset)Outputcontrol ="PS"+timestamp+control;
        for (int i = 0; i < 40; i++)
        {
          Serial.println(Outputcontrol);

          delay(500);
        }
        v1v=0; 
      

        
      }//if (timecount > timeperiod)

       
     // post_sensordata(post_dataV,0,v2v,v3v,v4v,v5v,v6v,timecount,v8v);

    

    }
}//if (WIFIconnected)  

else
{
  for (int j=0;j<4;j++) 
  {
 Serial.println("PX10101005");delay(1000);
  } 
 
}




}

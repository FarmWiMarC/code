//20190830-add scan serial port
#include <SPI.h>
#include <LoRa.h>
int maxloop=3;
int counter = 0;
int loopcount=0;
int packetSize=0;
char rfID='M';
int limit=30;
int e,scancount;
int loracount=0;
byte data[12];
char addr[8];
char netpiepublish[50];
char control[30];
int minState=0;
int BitcheckA=0xFF;
int BitcheckB=0xFF;
boolean flagSend = true;
boolean FlagRain=false;
boolean Flagreset = false;
int resetcount =0;

boolean flagRelay=true;
boolean flagserial=true;
boolean flagcontrol = false;
int timeONlimit=120;
int timeperiod=0;
int timeperiodserial=0;
int timeperiodlimit=60;
int loopnumber = 1000;

int i,j;
int Ptcount=0;
#define rxPin 12
#define txPin 11
#define rfPin 13
#define LORA_RESET 9
char InputRS232[40];
long Tempvoltage,Luxvoltage,Levelvoltage1,Levelvoltage2,winddirectionrika,Levelvoltage3;
long tempT,Lux,Level1,Level2,Level3,pressure;
int v1,v2,v3,v4,v5,v6,v7,v8,d1,d2,WindD;
int v1a,v2a,v3a,v4a,v5a,v6a,v7a,v8a;
int v1b,v2b,v3b,v4b,v5b,v6b,v7b,v8b;
//Addresses

//mois
float M;


int angle;
float OldTime;
int voltref=503;
int voltoffset=0;
float timecount =0;
float timeONcount =0;
float reftime =0;
float reftimeON =0;
int WindReading,intwind;
float WindChill;
char InputRPI[30];
char ValueSensor[50];

int indexchar;
//Anometer
volatile unsigned int windRotation = 0;
//Used for timing
float windTimer = 0;
float windDtime = 0;

float Calculation_temp;
int Calculation_temp1;

//Humidity sensor HIH5030
//Tempe sensor MCP9701 
//Humidity to A0
// Temperature to A1
unsigned long lastvalue;
unsigned long Supvoltage,RHvoltage,Windvoltage;
unsigned long RH = 0;
int sensorPin = 1; 
//int loopcount;
int inputcount=0;
//Rain gauge
unsigned long RainMeasurement = 0;
unsigned long LastRainReset = 0;
volatile byte Hit = 1;

byte CurrentDisplayPage = 0;
int relayPin = 4; //output pin 13

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
boolean FlagShutdown=false;
boolean FlagStart=false;
int RHint;
long temp;

/////////////////////////////////////////////////////////////
/////////////Serial
////////////////////////////////////////////////////////
void serialEvent() {
//void serialCheck() {
int i,j; 
 i=0;   
    Serial.println("--------->scan input"); 
     // for (i=1;i<5000;i++){ 
      while(!((InputRPI[0] == 'P')&&(InputRPI[1] == 'X'))|| !((InputRPI[0] == 'P')&&(InputRPI[1] == 'T'))
      || !((InputRPI[0] == 'P')&&(InputRPI[1] == 'S'))){  
        i++;
  char inChar = (char)Serial.read(); 
   //Serial.print(inChar);
     if ((inChar == 'P')) 
     { InputRPI[0]=inChar;
    for (j=1;j<30;j++){  InputRPI[j] = Serial.read();
      //if (InputRPI[j]=='?')break;
      
      }  
      //Serial.println(InputRPI);
       delay(100);
  
   if( ((InputRPI[0] == 'P')&&(InputRPI[1] == 'X')))
  { 
          Serial.end();
          digitalWrite(5, LOW);
          delay(5000);
          digitalWrite(5, HIGH);
       
          delay(3000);
          Serial.begin(9600);
  }
        
       
  if(( ((InputRPI[0] == 'P')&&(InputRPI[1] == 't')))||((InputRPI[0] == 'P')&&((InputRPI[1] == 'T')||(InputRPI[1] == 'S')))
  
    //||((((InputRPI[0] == 'P')&&(InputRPI[1] == 'C'))||((InputRPI[0] == 'P')&&(InputRPI[1] == 'c')) )))
    )

  {
   
      Serial.println(InputRPI);
     stringComplete=true;
     inputcount =0;   
         if ((InputRPI[9]=='c'))
     {    if (InputRPI[10]=='1') {reftimeON=millis();timeperiodserial=60;}
          if (InputRPI[10]=='0') timeperiodserial=0;
  
           if ((InputRPI[10]=='1')&&flagserial&&flagRelay)
                            {digitalWrite(4, HIGH);
                             reftimeON=millis();
                             flagRelay=false;
                             flagserial=false;
                             Serial.println("RELAY:  ON");
                            }
       break;
                 
                 }
    
    
     
     }
     
   
     
     
     
  

      } // if 
    if  (((InputRPI[0] == 'P')&&(InputRPI[1] == 'X')))
     limit=50;
     
       //if 
     if (i>10000){Serial.println("No input"); break;}
     delay(1);
     }//for 
  
  //  while (Serial.available()) 
  //  Serial.read();



  
}




///////////////////////////////////////////////////
void checkControl()
{
  if  (digitalRead(8)== HIGH) //control from ESP32
        {                   
                         if ((flagRelay))
                            {digitalWrite(4, HIGH);
                             reftimeON=millis();
                             flagRelay=false;
                             flagserial=false;
                             timeperiodserial=60;
                             Serial.println("RELAY:  ON-D8 HIGH");
                            }     
          
        
               
             
       }

  
  }












//////////////////////////////////////////////////////////////////

boolean ConvertData_CheckModeID(int measPoint)
{char i,data[3],checkchar;
 boolean FlagSearch;
 int tmp;
 char  Data1,Data2,Data3,Data4,Data5;
 int tmp1,tmp2;

 
 FlagSearch =1;i=0;//k=0;

//char x=InputRS232[1]+InputRS232[4]+InputRS232[7]+InputRS232[10]+InputRS232[13]+InputRS232[16]+InputRS232[19]+InputRS232[22]+InputRS232[25]+InputRS232[28]+InputRS232[31];
char x=InputRS232[1]+InputRS232[4]+InputRS232[7]+InputRS232[10]+InputRS232[13]+InputRS232[16]+InputRS232[19]+InputRS232[22]+InputRS232[25];

   if (InputRS232[26] != x)    {
//    char x =InputRS232[5]+InputRS232[8]+InputRS232[1]+InputRS232[2];
    Serial.print("Input ="); Serial.println(InputRS232[26]);
    Serial.print("check ="); Serial.println(x);
    delay(1000);
    
    return false;  
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
tmp1 = v1/10000;                tmp2 = v1%10000; 
       Data1 = tmp1;
       tmp1 = tmp2/1000;                     tmp2 %= 1000;               
       Data2 = tmp1;
       tmp1 = tmp2/100;                     tmp2 %= 100;               
       Data3 = tmp1;
       tmp1 = tmp2/10;                     tmp2 %= 10;               
       Data4 = tmp1;
              
       Data5 = tmp2;
       
       Data1 += 0x30;  Data2 += 0x30;  Data3 += 0x30; Data4 += 0x30;  Data5 += 0x30;    
//ValueSensor[2] = 0x41; // "A"
netpiepublish[0] = Data1;
netpiepublish[1] = Data2;
netpiepublish[2] = Data3;
netpiepublish[3] = Data4;
netpiepublish[4] = Data5;
netpiepublish[5] = ';';

}

 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { Serial.println("v1 error");return false;}



 



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

tmp1 = v2/10000;                tmp2 = v2%10000; 
       Data1 = tmp1;
       tmp1 = tmp2/1000;                     tmp2 %= 1000;               
       Data2 = tmp1;
       tmp1 = tmp2/100;                     tmp2 %= 100;               
       Data3 = tmp1;
       tmp1 = tmp2/10;                     tmp2 %= 10;               
       Data4 = tmp1;
              
       Data5 = tmp2;
       
       Data1 += 0x30;  Data2 += 0x30;  Data3 += 0x30; Data4 += 0x30;  Data5 += 0x30;    
//ValueSensor[2] = 0x41; // "A"
netpiepublish[6] = Data1;
netpiepublish[7] = Data2;
netpiepublish[8] = Data3;
netpiepublish[9] = Data4;
netpiepublish[10] = Data5;
netpiepublish[11] = ';';

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { Serial.println("v2 error");return false;}

 
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
tmp1 = v3/10000;                tmp2 = v3%10000; 
       Data1 = tmp1;
       tmp1 = tmp2/1000;                     tmp2 %= 1000;               
       Data2 = tmp1;
       tmp1 = tmp2/100;                     tmp2 %= 100;               
       Data3 = tmp1;
       tmp1 = tmp2/10;                     tmp2 %= 10;               
       Data4 = tmp1;
              
       Data5 = tmp2;
       
       Data1 += 0x30;  Data2 += 0x30;  Data3 += 0x30; Data4 += 0x30;  Data5 += 0x30;    
//ValueSensor[2] = 0x41; // "A"
netpiepublish[12] = Data1;
netpiepublish[13] = Data2;
netpiepublish[14] = Data3;
netpiepublish[15] = Data4;
netpiepublish[16] = Data5;
netpiepublish[17] = ';';

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { Serial.println("v3 error");return false;}

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

tmp1 = v4/10000;                tmp2 = v4%10000; 
       Data1 = tmp1;
       tmp1 = tmp2/1000;                     tmp2 %= 1000;               
       Data2 = tmp1;
       tmp1 = tmp2/100;                     tmp2 %= 100;               
       Data3 = tmp1;
       tmp1 = tmp2/10;                     tmp2 %= 10;               
       Data4 = tmp1;
              
       Data5 = tmp2;
       
       Data1 += 0x30;  Data2 += 0x30;  Data3 += 0x30; Data4 += 0x30;  Data5 += 0x30;    
//ValueSensor[2] = 0x41; // "A"
netpiepublish[18] = Data1;
netpiepublish[19] = Data2;
netpiepublish[20] = Data3;
netpiepublish[21] = Data4;
netpiepublish[22] = Data5;
netpiepublish[23] = ';';

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { Serial.println("v4 error");return false;}


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
tmp1 = v5/10000;                tmp2 = v5%10000; 
       Data1 = tmp1;
       tmp1 = tmp2/1000;                     tmp2 %= 1000;               
       Data2 = tmp1;
       tmp1 = tmp2/100;                     tmp2 %= 100;               
       Data3 = tmp1;
       tmp1 = tmp2/10;                     tmp2 %= 10;               
       Data4 = tmp1;
              
       Data5 = tmp2;
       
       Data1 += 0x30;  Data2 += 0x30;  Data3 += 0x30; Data4 += 0x30;  Data5 += 0x30;    
//ValueSensor[2] = 0x41; // "A"
netpiepublish[24] = Data1;
netpiepublish[25] = Data2;
netpiepublish[26] = Data3;
netpiepublish[27] = Data4;
netpiepublish[28] = Data5;
netpiepublish[29] = ';';

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { Serial.println("v5 error");return false;}

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
tmp1 = v6/10000;                tmp2 = v6%10000; 
       Data1 = tmp1;
       tmp1 = tmp2/1000;                     tmp2 %= 1000;               
       Data2 = tmp1;
       tmp1 = tmp2/100;                     tmp2 %= 100;               
       Data3 = tmp1;
       tmp1 = tmp2/10;                     tmp2 %= 10;               
       Data4 = tmp1;
              
       Data5 = tmp2;
       
       Data1 += 0x30;  Data2 += 0x30;  Data3 += 0x30; Data4 += 0x30;  Data5 += 0x30;    
//ValueSensor[2] = 0x41; // "A"
netpiepublish[30] = Data1;
netpiepublish[31] = Data2;
netpiepublish[32] = Data3;
netpiepublish[33] = Data4;
netpiepublish[34] = Data5;
netpiepublish[35] = ';';

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else { Serial.println("v6 error");return false;}



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
tmp1 = v7/10000;                tmp2 = v7%10000; 
       Data1 = tmp1;
       tmp1 = tmp2/1000;                     tmp2 %= 1000;               
       Data2 = tmp1;
       tmp1 = tmp2/100;                     tmp2 %= 100;               
       Data3 = tmp1;
       tmp1 = tmp2/10;                     tmp2 %= 10;               
       Data4 = tmp1;
              
       Data5 = tmp2;
       
       Data1 += 0x30;  Data2 += 0x30;  Data3 += 0x30; Data4 += 0x30;  Data5 += 0x30;    
//ValueSensor[2] = 0x41; // "A"
netpiepublish[36] = Data1;
netpiepublish[37] = Data2;
netpiepublish[38] = Data3;
netpiepublish[39] = Data4;
netpiepublish[40] = Data5;
netpiepublish[41] = ';';

 }
 // if (data[2] == (InputRS232[3]&&InputRS232[4]) )
 else {Serial.println("v7 error"); return false;}

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
tmp1 = v8/10000;                tmp2 = v8%10000; 
       Data1 = tmp1;
       tmp1 = tmp2/1000;                     tmp2 %= 1000;               
       Data2 = tmp1;
       tmp1 = tmp2/100;                     tmp2 %= 100;               
       Data3 = tmp1;
       tmp1 = tmp2/10;                     tmp2 %= 10;               
       Data4 = tmp1;
              
       Data5 = tmp2;
       
       Data1 += 0x30;  Data2 += 0x30;  Data3 += 0x30; Data4 += 0x30;  Data5 += 0x30;    
//ValueSensor[2] = 0x41; // "A"
netpiepublish[42] = Data1;
netpiepublish[43] = Data2;
netpiepublish[44] = Data3;
netpiepublish[45] = Data4;
netpiepublish[46] = Data5;
netpiepublish[47] = ';';

 }
else {Serial.println("v8 error"); return false;}

if (InputRS232[1] == 'A')
{
  v1a=v1;
  v2a=v2;
  v3a=v3;
  v4a=v4;
  v5a=v5;
  v6a=v6;
  v7a=v7;
  v8a=v8;
  
  }

if (InputRS232[1] == 'B')
{
  v1b=v1;
  v2b=v2;
  v3b=v3;
  v4b=v4;
  v5b=v5;
  v6b=v6;
  v7b=v7;
  v8b=v8;
  
  }


//     zProcessSendLORA(InputRS232[1]);

return true;


 
 
 
 
 
}


void Wind_Init(void)
{
  pinMode(3, INPUT);
  attachInterrupt(1, windSpeed, RISING);
  windTimer=millis();//start timing  
  
}

int pulseCount =0;
int sensorInterrupt=1;
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
int Wind_GetSpeed(void)
{
attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  float oldTime=millis();
   while((millis() - oldTime) < 2000)    // Only process counters once per second
  { delay(1); 
  } 
    
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    if  (pulseCount < 3) pulseCount =0;
    pulseCount=pulseCount*100;
    Serial.print("pulse:");Serial.println(pulseCount);
   int flowRate = ((2000.0 / (millis() - oldTime)) * pulseCount) / 1;
    //flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;


    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
  //  flowMilliLitres = (flowRate / 1 ) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
  //  totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Pulse: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("pulse");
    Serial.print("\t");       // Print tab space

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
   
  // flowRate = flowRate*2/3;//1 rotation per second equals 2.4 km/h = 2/3 m/s  
  return flowRate;
   
}

void windSpeed()
{
  windRotation++;
}

void WindDirection(byte y)
{ int adc;
    // read the analog input into a variable:
   
 Windvoltage = analogRead(2);
 //adc = get_wind_direction(Windvoltage);
 
 
 
  Windvoltage=  Windvoltage*voltref;
  Windvoltage=Windvoltage/102.3-voltoffset;
   WindD=360*Windvoltage/5000;
  
    Serial.print("Windvoltage =");
    Serial.print(Windvoltage);//print the result
    Serial.println(" mV");//print the result 
 
 
   //ProcessConvertValueToRPI('W','A',adc,Windvoltage);
   //Serial.println(ValueSensor);
   //  Serial.println();
   
 
  
}  




void Rain_Init(void)
{
  attachInterrupt(0, Rain_Measure, RISING);
  //LastRainReset = millis();
 // LCD_CenterText("* Regn", 54);
}    

void Rain_Measure(void)
{if (FlagRain)
{
  RainMeasurement = RainMeasurement + 20; 
} 
}

////////////////////////////////////////////////
/////////////Convert data send with LORA
///////////////////


void ProcessSendLORA(char idex)
{ 
int i;
int Data1,Data2,Data3;
   int tmp;
char sum1,sum2,sum3,sum4,sum5,sum6,sum7,sum8,sum9,sum10;
ValueSensor[0] = 'P';
ValueSensor[1] = idex;
//////////////////v1 Send
 

 
 tmp =v1+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =v1+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;



  
sum1=idex+Data1+Data2;
Data3=Data1+Data2;


//Data1='A';

ValueSensor[2] = Data1;
ValueSensor[3] = Data2;
ValueSensor[4] = sum1;

//////////////////v2 Send
  tmp =v2+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =v2+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;
  
sum2=idex+Data1+Data2;

Data3=Data1+Data2;
ValueSensor[5] = Data1;
ValueSensor[6] = Data2;
ValueSensor[7] = sum2;
//////////////////v3 Send
  tmp =v3+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =v3+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;
sum3=idex+Data1+Data2;


Data3=Data1+Data2;

ValueSensor[8] = Data1;
ValueSensor[9] = Data2;
ValueSensor[10] = sum3;
//////////////////v4 Send
 tmp =v4+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =v4+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;
sum4=idex+Data1+Data2;
Data3=Data1+Data2;
ValueSensor[11] = Data1;
ValueSensor[12] = Data2;
ValueSensor[13] =sum4;
//////////////////v5 Send
  tmp =v5+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =v5+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;
sum5=idex+Data1+Data2;
Data3=Data1+Data2;
ValueSensor[14] = Data1;
ValueSensor[15] = Data2;
ValueSensor[16] = sum5;
//////////////////v6 Send
 tmp =v6+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =v6+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;
sum6=idex+Data1+Data2;
Data3=Data1+Data2;
ValueSensor[17] = Data1;
ValueSensor[18] = Data2;
ValueSensor[19] =sum6;
//////////////////v7 Send
 tmp =v7+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =v7+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;
sum7=idex+Data1+Data2;
Data3=Data1+Data2;
ValueSensor[20] = Data1;
ValueSensor[21] = Data2;
ValueSensor[22] = sum7;


//////////////////v8 Send
 tmp =v8+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =v8+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;
sum8=idex+Data1+Data2;

Data3=Data1+Data2;
ValueSensor[23] = Data1;
ValueSensor[24] = Data2;
ValueSensor[25] = sum8;




//////////////////d1 Send
 tmp =d1+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =v8+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;
sum9=idex+Data1+Data2;

Data3=Data1+Data2;
ValueSensor[27] = Data1;
ValueSensor[28] = Data2;
ValueSensor[29] = sum9;

//////////////////d2 Send
 tmp =d2+1000; 
  tmp = tmp & 0xff00;
  tmp >>= 8; 
  Data1 = tmp;

  
 tmp =d2+1000; 
 
  tmp = tmp & 0x00ff;
  Data2 = tmp;
sum10=idex+Data1+Data2;

Data3=Data1+Data2;
ValueSensor[30] = Data1;
ValueSensor[31] = Data2;
ValueSensor[32] = sum10;


ValueSensor[26]=ValueSensor[1]+ValueSensor[4]+ValueSensor[7]+ValueSensor[10]+ValueSensor[13]+ValueSensor[16]+ValueSensor[19]+ValueSensor[22]+ValueSensor[25];
//ValueSensor[32]=idex+sum1+sum2+sum3+sum4+sum5+sum6+sum7+sum8+sum9+sum10;//
 
//ValueSensor[27]='\n';

//for (i=1;i<30;i++) { Serial.print(ValueSensor[i],HEX); Serial.print(','); }   
for (i=0;i<5;i++){
  for (int j=0;j<30;j++)
  Serial.print(ValueSensor[j]); 
  Serial.println();
  delay(500);
}



}

void checkBit(char ID)
{//Serial.println("checkbit: ");Serial.println(ID);
  switch (ID){
    case 'A': BitcheckA = BitcheckA & B01111111; break;
    case 'B': BitcheckA = BitcheckA & B10111111; break;
    case 'C': BitcheckA = BitcheckA & B11011111; break;
    case 'D': BitcheckA = BitcheckA & B11101111; break;
    case 'E': BitcheckA = BitcheckA & B11110111; break;
    case 'F': BitcheckA = BitcheckA & B11111011; break;
    case 'G': BitcheckA = BitcheckA & B11111101; break;
    case 'H': BitcheckA = BitcheckA & B11111110; break;
    case 'I': BitcheckB = BitcheckB & B01111111; break;
    case 'J': BitcheckB = BitcheckB & B10111111; break;
    case 'K': BitcheckB = BitcheckB & B11011111; break;
    case 'L': BitcheckB = BitcheckB & B11101111; break;
    case 'M': BitcheckB = BitcheckB & B11110111; break;
    case 'N': BitcheckB = BitcheckB & B11111011; break;
    case 'O': BitcheckB = BitcheckB & B11111101; break;
    case 'P': BitcheckB = BitcheckB & B11111110; break;
    }
  
  
  }


boolean flagpower=false;


void setup() {

v1a=v2a=v3a=v4a=v5a=v6a=v7a=v8a=0;
v1b=v2b=v3b=v4b=v5b=v6b=v7b=v8b=0;
 
 pinMode(4, OUTPUT);
 pinMode(7, OUTPUT);digitalWrite(7, LOW);
  pinMode(9, OUTPUT);digitalWrite(9, LOW);
Supvoltage = analogRead(7);
 Supvoltage = Supvoltage*voltref;
Supvoltage = Supvoltage/102.3-voltoffset;
Supvoltage = Supvoltage*11+700;

if (Supvoltage>4000)
 {
  delay(5000);
  //digitalWrite(6, LOW);
  
 // digitalWrite(9, HIGH);
 // delay(3000);
  digitalWrite(9, LOW);
          delay(5000);
   digitalWrite(9, HIGH);
     delay(3000);
    
    digitalWrite(4, HIGH);
 // delay(3000);           
 //
 
  Serial.begin(9600);

   //   delay(3000);
    if (!LoRa.begin(925E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
 }
  
        
  LoRa.setTxPower(20);
 Serial.println("STRARTING LoRa ");
 flagpower= true;
 
 int x=0;
   while (Serial.available())
    
    { char inChar = (char)Serial.read(); 
     Serial.print(inChar);
     if ((inChar == 'P'))
         {
         Serial.print(x);break;}
     x++;
     
     if (x>100000)
     {
      break;
     }
    delay(1);
    }
 
 
 
 }
else
{
 
  Serial.begin(9600);
  Serial.println("Low Power mode");
}
 
//pinMode(4, OUTPUT);digitalWrite(4, LOW);
pinMode(8, OUTPUT);
  
/////////////////////////////////


 //Wind_Init();
  Rain_Init();
  FlagRain=true;
  delay(1000);
  pinMode(relayPin, OUTPUT); // output
  pinMode(5, OUTPUT);
  loopcount=0;
  WindReading=0;
  
  RHint=0;
  temp=0;
  reftime = millis();     
  RainMeasurement = 0;
 //  RHint=0;
 RHvoltage = analogRead(0);
 RHvoltage = RHvoltage*voltref;
 RHvoltage = RHvoltage/102.3-voltoffset;

Tempvoltage = analogRead(1); 
 Tempvoltage = Tempvoltage*voltref;
Tempvoltage = Tempvoltage/102.3-voltoffset;

Windvoltage = analogRead(2); 
 Windvoltage = Windvoltage*voltref;
Windvoltage = Windvoltage/102.3-voltoffset;
 

 
Luxvoltage = analogRead(3); 
 Luxvoltage = Luxvoltage*voltref;
Luxvoltage = Luxvoltage/102.3-voltoffset;

Levelvoltage1 = analogRead(4); 
 Levelvoltage1 = Levelvoltage1*voltref;
Levelvoltage1 = Levelvoltage1/102.3-voltoffset;

Levelvoltage2 = analogRead(5); 
 Levelvoltage2 = Levelvoltage2*voltref;
Levelvoltage2 = Levelvoltage2/102.3-voltoffset;

Levelvoltage3 = analogRead(6); 
 Levelvoltage3 = Levelvoltage3*voltref;
Levelvoltage3 = Levelvoltage3/102.3-voltoffset;


  Supvoltage = analogRead(7);
 Supvoltage = Supvoltage*voltref;
Supvoltage = Supvoltage/102.3-voltoffset;
Supvoltage = Supvoltage*11+700;

  reftime =millis();
 reftimeON=millis();
/////////////////





  /////////////////////////////////////////////////////////
 



limit=30;

////////////////

bitSet(addr[1],0);
bitSet(addr[1],1);
bitSet(addr[1],2);
bitSet(addr[1],3);
bitSet(addr[1],4);
bitSet(addr[1],5);
bitSet(addr[1],6);
bitSet(addr[1],7);

  ////////////////////////////////////////////////////////
  
  





  
}

void loop() {
  //serialCheck(); 
inputcount +=1;
flagpower = true;
//checkControl();
   ///////////////////
if ((flagpower==false))
{ 
 inputcount =0;
 loracount =0;
 Serial.println("Low power mode");
  
     if (Supvoltage>4000)
    {
     flagpower=true;  
  
      Serial.end();
          digitalWrite(9, LOW);
          delay(5000);
          digitalWrite(9, HIGH);
       
          delay(3000);
          Serial.begin(9600);
    
 int x=0;
   while (true)
    
    { char inChar = (char)Serial.read(); 
     Serial.print(inChar);
     if ((inChar == 'P'))
         break;
     x++;
    
     if (x>100000)
     {
      break;
     }
    
    }


          
    if (!LoRa.begin(925E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
    }
  
        
  LoRa.setTxPower(20);
 Serial.println("STRARTING LoRa ");
    }
   
}
if (stringComplete&&(FlagStart == false)) 
     {
   inputcount =0;   
   Serial.print("serial input = ");    Serial.println(InputRPI);

if  (((InputRPI[0] == 'P')&&(InputRPI[1] == 'X')))
     {   
      
     
     // digitalWrite(relayPin, LOW);  
      digitalWrite(5, LOW);
       
      delay(5000);
      ///////////////
     
   pinMode(5, OUTPUT); 
   digitalWrite(5, HIGH);
  
    
     }

   //////////////////  
    if ((InputRPI[1] == 'T')|| ((InputRPI[1] == 'S')))
    {//inputString ="";
     //inputString.toCharArray(InputRPI, 15);
  //reftime =millis();
      inputcount =0;
      loopcount =1; 
       Serial.flush();
       if (((InputRPI[0] == 'P')&&(InputRPI[1] == 'S')))
    { 
    for (j=0;j<10;j++)
      {
        Serial.println(InputRPI); 
        delay (500);
      }
                    WindReading=0;
                    RainMeasurement = 0;
                     int  RHint=0;
                     int temp=0;
                     loopcount =1; 
                     
                     Ptcount=0;
                      reftime =millis();
                    //FlagShutdown =true;
                    Flagreset =false;
                     Serial.flush(); 
                     
                     //digitalWrite(relayPin, LOW);
                     delay(3000);
                    
                     v1a=v2a=v3a=v4a=v5a=v6a=v7a=v8a=0;
                     v1b=v2b=v3b=v4b=v5b=v6b=v7b=v8b=0;
                   

                   
                    
                    FlagStart = false;                  
                  }

      stringComplete = false;
      for (j=0;j<30;j++){  InputRPI[j]= '0';  }
      while (Serial.available()) 
     Serial.read();
    }
     }//stringcomplete

/*//////////////////////////
v1= RHvoltage;    
v2=Tempvoltage;
v3=Luxvoltage ; 
v4=RainMeasurement;
v5=Levelvoltage2;
v6=Levelvoltage3;
v8=Windvoltage; 
v7=Supvoltage;


 d1=RainMeasurement; d2=windRotation;
 
// v1=100;v2=200;v1=100;v2=200;v1=100;v4=200;v5=100;v6=200;v7=100;
// d1=11111;d2=2222;
ProcessSendLORA('V');
  delay(1500);

Serial.print("Header:");Serial.println("V");
Serial.print("v1:");Serial.println(v1);
Serial.print("v2:");Serial.println(v2);
Serial.print("v3:");Serial.println(v3);
Serial.print("v4:");Serial.println(v4);
Serial.print("v5:");Serial.println(v5);
Serial.print("v6:");Serial.println(v6);
Serial.print("v7:");Serial.println(v7);
Serial.print("v8:");Serial.println(v8);


*//////////////read input///////////////////


float timecount = (millis()-reftime)/1000; 
Serial.print("timecount = "); Serial.println(timecount);
if ((timecount > (loracount*30))||(loracount > 4)) {
  BitcheckA=0xFF; BitcheckB=0xFF;
  }
  /////////////////////////////////////////////////////////
 
 // Wind direction measurement 
 WindDirection(36); 

 
 /////////////////////////////////////////////////////////
 
 
 
 ////////////////////////////////////////////////////////////
 // Wind direction measurement 
 
 ///////////////////////////////////////////////////////////
   
  
  lastvalue= WindReading;
  Serial.print("last Windspeed =");
  Serial.println(WindReading);  

//  delay(500);
  WindReading = Wind_GetSpeed();
    
  WindReading = (lastvalue+WindReading)/2;
  
      delay(50);
      Serial.print("Windspeed =  ");
      Serial.print(WindReading);    
      Serial.println(" cm/s");   
  
 //  Serial.println(ValueSensor);    
    Serial.println(); 
     // delay(3000);
 //  Print_WindDirection(36); 
  
  
  delay(50); 
        
///////////////////////////////////////////////////////////////   
         
// Humidity measurement       
  
///////////////////////////////////////////////////////////////  

 lastvalue=RHvoltage;


  RHvoltage = analogRead(0);
 RHvoltage = RHvoltage*voltref;
 RHvoltage = RHvoltage/102.3-voltoffset;
 
 RHvoltage = (RHvoltage+lastvalue)/2;
 
 RH= (1250*RHvoltage)/voltref-1250;
if (RH > 10000)
RH=10000;


 
 

 
 Serial.print("Humid ="); 
 Serial.print(RHvoltage);
 
  
 Serial.println(" %"); 


// Serial.println(ValueSensor);
Serial.println(); 

delay(50);
 
/////////////////////////////////////////////////////////////////////

// ambient temperature measurement

//////////////////////////////////////////////////////////////////// 

 lastvalue=Tempvoltage;
 Tempvoltage = analogRead(1); 
 Tempvoltage = Tempvoltage*voltref;
Tempvoltage = Tempvoltage/102.3-voltoffset;
 
  
   Tempvoltage = (Tempvoltage+lastvalue)/2;
  tempT = (Tempvoltage*2187.5)/voltref-6687.5;
 
  Serial.print("Temp  ="); 
 
 
 Serial.print(Tempvoltage);
 Serial.println(" C");
 
 

// Serial.println(ValueSensor);

 delay(50);
 Serial.println();


/////////////////////////////////////////////////////////////////////

// Lux measurement

//////////////////////////////////////////////////////////////////// 

 lastvalue=Luxvoltage;
Luxvoltage = analogRead(3); 
 Luxvoltage = Luxvoltage*voltref;
Luxvoltage = Luxvoltage/102.3-voltoffset;
 
 
   Luxvoltage = (Luxvoltage+lastvalue)/2;
     Lux = (Luxvoltage*100) /440;
  Serial.print("Lux  ="); 
 
 
 Serial.print(Luxvoltage);
 Serial.println(" lLux");
 
 

// Serial.println(ValueSensor);

 delay(50);
 Serial.println();

/////////////////////////////////////////////////////////////////////

//
//Lux 100k
//////////////////////////////////////////////////////////////////// 



lastvalue=Levelvoltage1;

 
Levelvoltage1 = analogRead(4); 
 Levelvoltage1 = Levelvoltage1*voltref;
Levelvoltage1 = Levelvoltage1/102.3-voltoffset;
 
 
  //int temp=(int)(temperatureC*10);
 
   Levelvoltage1 = (Levelvoltage1+lastvalue)/2;
  // temp = (Levelvoltage1 - 500) /8.0;
  //  Level1=temp;
  //winddirectionrika = 360*Levelvoltage1/5000;
  //Serial.print("Level1 ="); Serial.print(Level1,1); Serial.println("");
 pressure = 500+(Levelvoltage1-500)*0.25;
  Serial.print(" pressure ="); Serial.print(pressure); Serial.println("");
 
 delay(500);

// Serial.println(ValueSensor);

  delay(50);
 Serial.println();

/////////////////////////////////////////////////////////////////////

//
//Level2
//////////////////////////////////////////////////////////////////// 



 lastvalue=Levelvoltage2;

 
Levelvoltage2 = analogRead(5); 

Serial.print("Level2  ="); 
 
 
 Serial.print(Levelvoltage2);
 Serial.println(" C");
 Levelvoltage2 = Levelvoltage2*voltref;
Levelvoltage2 = Levelvoltage2/102.3-voltoffset;
 
  Serial.print("Level2  ="); 
 
 
 Serial.print(Levelvoltage2);
 Serial.println(" C");
 
  //int temp=(int)(temperatureC*10);
 
   Levelvoltage2 = (Levelvoltage2+lastvalue)/2;
  // temp = (Levelvoltage2 - 500) /8.0;
 
 

 //Serial.println(ValueSensor);

  delay(50);
 Serial.println();

 
 //////////////////////////////////////////////////////////////

 /////////////////////////////////////////////////////////////////////

//
//Level3
//////////////////////////////////////////////////////////////////// 



 lastvalue=Levelvoltage3;

 
Levelvoltage3 = analogRead(6); 
Serial.print("Level3  ="); 
 
 
 Serial.print(Levelvoltage3);
 Serial.println(" C");
 
 Levelvoltage3 = Levelvoltage3*voltref;
Levelvoltage3 = Levelvoltage3/102.3-voltoffset;
 Serial.print("Level3  ="); 
 
 
 Serial.print(Levelvoltage3);
 Serial.println(" C");
  
 
 
  //int temp=(int)(temperatureC*10);
 
   Levelvoltage3 = (Levelvoltage3+lastvalue)/2;
 // temp = (Levelvoltage - 500) /8.0;
 
 

 //Serial.println(ValueSensor);

  delay(50);
// Serial.println();

 
 //////////////////////////////////////////////////////////////


 
 //////////////////////////////////////////////////////////////
 
 // Rain measurement ....................

///////////////////////////////////////////////////////////////// 
      Serial.print("Rain gauge = ");
      Serial.println(RainMeasurement);
  
 //  Serial.println(ValueSensor);
   Serial.println(); 

   //////////////////////////////////////////////////////////////
   
  // Supply measurement....................
 
 //////////////////////////////////////////////////////////////////
 
 
  lastvalue=Supvoltage;

  Supvoltage = analogRead(7);
 Supvoltage = Supvoltage*voltref;
Supvoltage = Supvoltage/102.3-voltoffset;
Supvoltage = Supvoltage*11+700;
 

 
   Supvoltage = (Supvoltage+lastvalue)/2;
   
  Serial.print("Supply  ="); 
 
 
 Serial.print(Supvoltage,1);
 Serial.println("V");

if (Supvoltage<4000)
    { flagpower = false;
    Serial.println("Low power mode");
    }
//////////////////////////////////////////////////////
 lastvalue=Windvoltage;
   
  Windvoltage = analogRead(2);
 Windvoltage = Windvoltage*voltref;
Windvoltage = Windvoltage/102.3-voltoffset;
//Supvoltage = Supvoltage*11+700;
 

 
   Windvoltage = (Windvoltage+lastvalue)/2;
   
  Serial.print("Windvoltage/a2  ="); 
 
 
 Serial.print(Windvoltage,1);
 Serial.println("V");
 


 
  delay(100);

v1=tempT;        
v2=RH;            
v3=RainMeasurement;       
v4=WindD;        
v5=WindReading;    
v6=pressure;       
v7=Level3;       
v8=Lux;        




Serial.print("Header:");Serial.println("M");
Serial.print("v1:");Serial.println(v1);
Serial.print("v2:");Serial.println(v2);
Serial.print("v3:");Serial.println(v3);
Serial.print("v4:");Serial.println(v4);
Serial.print("v5:");Serial.println(v5);
Serial.print("v6:");Serial.println(v6);
Serial.print("v7:");Serial.println(v7);
Serial.print("v8:");Serial.println(v8);


if (timeONcount > 0)   
{ 
v1= RHvoltage; 
v2=Tempvoltage;   
v3= WindReading;
v4= RainMeasurement; 
v5= Levelvoltage2;
v6= Levelvoltage3;
v8= Windvoltage; 
v7= Supvoltage;
//ProcessSendLORA(rfID);
   for (int j=0;j<5;j++)
 {
  Serial.print(timeONcount);
 Serial.print("...........send rfID:");
 Serial.println(rfID);
  LoRa.beginPacket();
  LoRa.print(ValueSensor);
  LoRa.print('@');
  LoRa.print(timeONcount);
  
   LoRa.print('@');
     
  for (int k=8;k<17;k++)  LoRa.print(InputRS232[k]);
  
  
  
  LoRa.endPacket();

  int randomdelay = random(500, 2000);
 Serial.println(randomdelay);
  delay(randomdelay);
 }
 
}

else
{
   
v1= RHvoltage;    
v2=Tempvoltage;
v3=Luxvoltage ; 
v4=RainMeasurement;
v5=Levelvoltage2;
v6=WindReading;
v8=Windvoltage; 
v7=Supvoltage;
}

if(flagserial==false)
{
 v1= RHvoltage; 
v2=Tempvoltage;   
v3= WindReading;
v4= RainMeasurement;
//v4=Levelvoltage1;
v5=Levelvoltage2;
v6=Levelvoltage3;
v8=Windvoltage; 
v7=Supvoltage; 
  
  
  }



ProcessSendLORA('V');
  delay(1500);

Serial.print("Header:");Serial.println("V");
Serial.print("v1:");Serial.println(v1);
Serial.print("v2:");Serial.println(v2);
Serial.print("v3:");Serial.println(v3);
Serial.print("v4:");Serial.println(v4);
Serial.print("v5:");Serial.println(v5);
Serial.print("v6:");Serial.println(v6);
Serial.print("v7:");Serial.println(v7);
Serial.print("v8:");Serial.println(v8);

if (flagpower)
{
///////////////Control /////////////////////

if (flagRelay==false)
 {


  if (flagserial == false)
  {

   timeONcount = (millis()-reftimeON)/1000;
 Serial.print("timeONcount=");

 Serial.println(timeONcount); 

 Serial.print("timeperiodserial=");

    
   Serial.println(timeperiodserial);
  //if ((InputRPI[10]=='0')&&(digitalRead(8) == LOW)) timeperiodserial=0;
  if ((timeONcount > timeperiodserial)) 
        {
          Serial.println("TIME OUT serial");
         // digitalWrite(4, LOW);
       if (InputRPI[10]=='0')
      {
         reftimeON=millis();
         flagRelay=true;
         flagserial= true;
         timeONcount =0;
         Serial.println("TimeON serial reset");
      
      }
     }
  }
  else
 {  
 timeONcount = (millis()-reftimeON)/1000;
 Serial.print("timeONcount=");

 Serial.println(timeONcount); 

 Serial.print("timeperiod=");
       
 Serial.println(timeperiod);
  if ((timeONcount > timeperiod)) 
        {
          Serial.println("TIME OUT");
         // digitalWrite(4, LOW);
          if (InputRPI[10]=='0')
      {
         reftimeON=millis();
         flagRelay=true;
         flagserial= true;
         timeONcount =0;
         Serial.println("TimeON reset");
      
      }
        }
  
 
 }
 }


//checkControl();


/////////////////////////////////////////////////  

Serial.println(BitcheckA,BIN);
Serial.println(BitcheckA&0b00001111,BIN);
Serial.println(BitcheckA>>4,BIN);
addr[0]='I';

addr[1]=(BitcheckA>>4)+'0';
addr[2]=(BitcheckA&0b00001111)+'0';
addr[3]=addr[1]+addr[2];
addr[4]=(BitcheckB>>4)+'0';
addr[5]=(BitcheckB&0b00001111)+'0';
addr[6]=addr[4]+addr[5];

if ((addr[2]=='0') && (addr[3]=='0'))
 {BitcheckA=0xff;
 }
if ((addr[4]=='0') && (addr[5]=='0'))
 {BitcheckB=0xff;
 }
if (flagcontrol){maxloop=loracount*3;loracount =2; flagcontrol =false;}
if ((loracount%2 ==0))
//if ((loracount%2 ==0))
{
 
  
 for( scancount=0; scancount<maxloop;scancount++)
 {
 //  for(int j=0; j<3;j++) {Serial.print(addr[j],BIN);Serial.print(',');Serial.println();}
 Serial.print(addr); Serial.println(control);
 LoRa.beginPacket();
  LoRa.print(addr);
   LoRa.print(control);
 // LoRa.print(counter);
  LoRa.endPacket();
   if (stringComplete)
{
  stringComplete=false;
  Serial.println(InputRPI);
    InputRPI[0]= '0';
   while (Serial.available()) 
    Serial.read();
}



  
 int randomdelay = random(500, 2000);
 Serial.println(randomdelay);
  delay(randomdelay);
 } 
}
  Serial.print("waiting for Input packet loopcount ");
   
   
  
   int loopnumber = 500;
    if (loracount > 5) loopnumber = loracount*100;
    if (loopnumber > 1000) loopnumber = 1500;
  
  while ((packetSize == 0)&&(loopcount < loopnumber)) 
  {
     
      if ((stringComplete)&&(loracount <5))
       {
           stringComplete=false;
           Serial.println(InputRPI);
           if (InputRPI[1]=='S')
              for (int s=0;s<10;s++)
              { Serial.println(InputRPI);delay(500);
                }
           InputRPI[0]= '0';
          while (Serial.available()) 
          Serial.read();
          break;
       }
    
    //////////////read input///////////////////
     //checkControl();
     
     if ((flagserial == false)&&(flagRelay == false)&&(InputRPI[10]=='0'))
       { //digitalWrite(4, LOW);
         //flagRelay=true;
         //flagserial=true;
         timeperiodserial=0;
         Serial.println("RELAY:  OFF->D8 LOW");
         break;
        }      
        
        loopcount++;
         if(loopcount%100 == 0)
         {Serial.print(".");Serial.print(loopnumber);
         }
         packetSize = LoRa.parsePacket();
         // received a packet
             if ((packetSize > 0))
             { 
             Serial.println("Received packet: '");
    
            // read packet
  
            int i=0;
        
            while (LoRa.available()) {
                InputRS232[i]=((char)LoRa.read());
                i++;
              }
           Serial.println(InputRS232);
            if((InputRS232[8] == 'P')&&((InputRS232[9] == '@')))
            {
            digitalWrite(5, LOW); 

            for (int j=0;j<5;j++)
            {
            Serial.print("...........send rfID:");
            Serial.println(rfID);
            LoRa.beginPacket();
            LoRa.print("P");
            LoRa.print('@');
            LoRa.print(rfID);
            LoRa.print('@');
            for (int k=0;k<10;k++)  LoRa.print(InputRPI[k]);
            LoRa.endPacket();
            int randomdelay = random(500, 2000);
            Serial.println(randomdelay);
            delay(randomdelay);

 
          }//for
     
          delay(5000);
          digitalWrite(5, HIGH); 
          }
  
   
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    Serial.print(" loopcount ");
    Serial.println(loopcount);
    Serial.print(" packetSize  ");
    Serial.println(packetSize);
    //loracount =0;


    if (ConvertData_CheckModeID(0))
   {
  
     checkBit(InputRS232[1]);
    Serial.print(F("Input ID"));
    Serial.println(BitcheckA,BIN);

  
    Serial.print(F("netpie: "));
    Serial.println(netpiepublish);
    for (int j=0;j<5;j++) 
    {for (int k=0;k<30;k++) 
      Serial.print(InputRS232[k]);
    
    Serial.println();
    delay(500);
    }
    loracount=1;  
    reftime=millis();

    }

 
  }
  
  delay(10);
  }//while
  loracount++;
  Serial.print("loracounnt: ");Serial.println(loracount);

  //while
  inputcount++;

     Serial.print("inputcount=");Serial.println(inputcount);
// Serial.print(F("loracount: "));    Serial.println(loracount);
 
// if (Ptcount < 0){ LoraSetup(); Ptcount=0; }
     
  
//   if( (loracount%2500 ==0)||(inputcount%10 ==0)||(inputcount < 0))
if( (loracount%2500 ==0)||(inputcount%1000 ==0)||(inputcount < 0))
  {
  int x=0;
   
   Serial.end();
         // digitalWrite(6, HIGH);
   digitalWrite(4, LOW);
   digitalWrite(9, LOW);
   delay(5000);
  /////
   
   digitalWrite(9, HIGH);
     delay(3000);
    digitalWrite(4, HIGH);
 // delay(3000);           
 //
  
                
 //delay(3000);
  
  Serial.begin(9600);

   //   delay(3000);
    if (!LoRa.begin(925E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
 }
  
        
  LoRa.setTxPower(20);
 Serial.println("STRARTING LoRa ");
 flagpower= true;
 
 x=0;
   while (Serial.available())
    
    { char inChar = (char)Serial.read(); 
     Serial.print(inChar);
     if ((inChar == 'P'))
       {
         Serial.print(x);break;}
         
     x++;
     
     if (x>100000)
     {
      break;
     }
    delay(1);
    }
  /////
 Serial.println("RESET LoRa ");
}
  
////////////////////////////////////////////  
////////////////////////////

  
  
  
  
  if ((loracount > 10000)||(inputcount > 50))
  { digitalWrite(5, LOW); // output pin 13
    delay(5000);
    digitalWrite(5, HIGH);
     
  }
loopcount=0;
packetSize=0;
delay(random(3000, 5000));

}


}



  


 

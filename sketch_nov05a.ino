#include <Narcoleptic.h>

#include "RAK811.h"
#include "SoftwareSerial.h"
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
#define JOIN_MODE ABP    //  OTAA or ABP
String NwkSKey = "D3DE4D44E864E6091C86C5D4C75EF5A2";
String AppSKey = "0922FEE0384AD556440B6D19E319F2FA";
String DevAddr = "26011F10";
#define TXpin 11   // Set the virtual serial port pins
#define RXpin 10
#define ATSerial Serial
SoftwareSerial DebugSerial(RXpin,TXpin);    // Declare a virtual serial port
char* buffer = "AABBCC";

bool InitLoRaWAN(void);
RAK811 RAKLoRa(ATSerial,DebugSerial);
  
  //sonar 1
  int vcc = 4;
  const int pwPin1 = 3;
  long sensor, mm, inches;
  //sonar 2
  int vcc_1 = 5; 
  int echoPin = 7; 
  int trigPin = 6; 

  String stringOne_s2 = ""; 
  String v1_s2 = "";
  String v2_s2 = "";
        
  String stringOne = "";
  String v1 = "";
  String v2 = "";
    
  String stringOne_s1_s2 =  "";

void setup() {
    

  
 DebugSerial.begin(115200);
    pinMode(pwPin1, INPUT);
    //sonar 1 живлення і сигнали
    pinMode(vcc, OUTPUT);
    //sonar 2  живлення і сигнали
    pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT); 
    pinMode(vcc_1, OUTPUT);
    
 while(DebugSerial.read()>= 0) {}
 while(!DebugSerial);
 ATSerial.println("StartUP");

 ATSerial.begin(115200); // Note: Please manually set the baud rate of the WisNode device to 9600.
 delay(100);
 ATSerial.println(RAKLoRa.rk_getVersion());
 delay(700);
 ATSerial.println(RAKLoRa.rk_getBand());
 delay(700);

 while (!InitLoRaWAN());

}

bool InitLoRaWAN(void)
{
  if (RAKLoRa.rk_setWorkingMode(WORK_MODE))
   delay(700);
  {
    if (RAKLoRa.rk_initABP(DevAddr, NwkSKey, AppSKey))
    delay(700);
    {
      ATSerial.println("You init ABP parameter is OK!");
      if (RAKLoRa.rk_joinLoRaNetwork(JOIN_MODE))
      delay(700);
      {
        ATSerial.println("You join Network success!");
        ATSerial.println(RAKLoRa.rk_setRate(5));
        delay(2000);
        return true;
     }
   }
  return false;
 }
}
void loop() {
  //датчик ультразвуку включили довгий з кабельом 
          int duration, cm; 
          digitalWrite(vcc_1, HIGH); 
          delay(200);
          digitalWrite(trigPin, LOW); 
          delayMicroseconds(2); 
          digitalWrite(trigPin, HIGH); 
          delayMicroseconds(10); 
          digitalWrite(trigPin, LOW); 
          duration = pulseIn(echoPin, HIGH); 
          cm = duration / 58;
          Serial.println(cm); 
          //Serial.println(" cm sonar 2"); 
          //Serial.print(cm, HEX); 
          //Serial.println(" cm sonar 2 hex"); 
          delay(100);
          digitalWrite(vcc_1, LOW); 
          delay(200);
  //переробити рефакторинг
          stringOne_s2 =  String(cm, HEX); 
           v1_s2 = stringOne_s2.substring(0, 2);
           v2_s2 = stringOne_s2.substring(2, 4);
              if(cm > 255){
                stringOne_s2 = "03" + v1_s2 + v2_s2 + "0";
              }else if(cm <= 255 && cm > 0){
               stringOne_s2 = "02" + v1_s2 + "00";
              }else if(cm < 0 ){
               stringOne_s2 = "E1";
              }else{
                stringOne_s2 = "E2";
              }

          //датчик ультразвуку включили короткий
          digitalWrite(vcc, HIGH); 
          delay(400);
          
          sensor = pulseIn(pwPin1, HIGH);
          mm = sensor;
          inches = mm/10;
          Serial.print("S1");
          Serial.print("=");
          Serial.println(inches);
          delay(100);
          
          digitalWrite(vcc, LOW);
          delay(400);

          stringOne =  String(inches, HEX); 
          v1 = stringOne.substring(0, 2);
          v2 = stringOne.substring(2, 4);
              if(inches > 255){
                stringOne = "8803" + v1 + v2 + "0";
              }else if(inches <= 255 && inches > 0){
               stringOne = "8802" + v1 +"00";
              }else{
                stringOne = "88E4"; 
              }



           
         stringOne_s1_s2 =  stringOne_s2 + stringOne;
         buffer = (char*) stringOne_s1_s2.c_str();

  ATSerial.println(RAKLoRa.rk_setRate(5));
        delay(2000);
  ATSerial.println(RAKLoRa.rk_sendData(0, 1, buffer));

   delay(2000);
  //int sen = RAKLoRa.rk_sendData(0, 1, buffer); 
  RAKLoRa.rk_sleep();
   delay(2000);
   
   for (int i=0; i <= 30; i++){
    Narcoleptic.delay(20000);  
   }
    delay(4000);
    //wake up Rak811
    ATSerial.println("at+set_config=device:sleep:0"); 
    delay(7000);
}

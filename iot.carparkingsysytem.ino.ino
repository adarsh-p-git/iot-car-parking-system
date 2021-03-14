#include <ESP8266WiFi.h>
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>;
#include <WiFiUdp.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
const char *ssid =  "wifi-name";     // Enter your WiFi Name
const char *pass =  "wifi-password"; // Enter your WiFi Password

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "adafruit-username"
#define MQTT_PASS "adafruit-aio-key"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800,60000);
Servo myservo;                          //servo as gate
Servo myservos;                               //servo as gate
#define led1 D4
#define led2 D6
#define led3 D7 //This LED,D7 pin is not used in this project
#define led4 D8

int carEnter = D0;                      // entry sensor 
int slot1 = D1;                 
int slot2 = D2;


int count =0; 
int CLOSE_ANGLE = 80;  // The closing angle of the servo motor arm
int OPEN_ANGLE = 0;  // The opening angle of the servo motor arm              
int  hh, mm, ss;
int pos;
int pos1;
 void MQTT_connect();
 int newcount;
 String str;
 String newstr;

String h, m,EntryTimeSlot1,ExitTimeSlot1, EntryTimeSlot2,ExitTimeSlot2, EntryTimeSlot3,ExitTimeSlot3;
boolean entrysensor,s1,s2,s3;

boolean s1_occupied = false;
boolean s2_occupied = false;
boolean s3_occupied = false;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

//Set up the feed you're subscribing to

//Set up the feed you're publishing to
Adafruit_MQTT_Publish CarsParked = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/carsparked");
Adafruit_MQTT_Publish EntrySlot1 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/entryslot1");
Adafruit_MQTT_Publish ExitSlot1 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/exitslot1");
Adafruit_MQTT_Publish EntrySlot2 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/entryslot2");
Adafruit_MQTT_Publish ExitSlot2 = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/exitSlot2");
Adafruit_MQTT_Publish slotstatus = Adafruit_MQTT_Publish(&mqtt,MQTT_NAME "/f/slotstatus");

void setup() {
  delay(1000);
  Serial.begin (9600); 
  timeClient.begin(); 
  myservos.attach(D5);       // servo pin to D5
   pinMode(carEnter, INPUT);     // ir as input
  pinMode(slot1, INPUT);
  pinMode(slot2, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led4,OUTPUT);
  WiFi.begin(ssid, pass);                                     //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(ssid);                          // display ssid
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");                          // if not connected print this
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
}

void loop() {

 MQTT_connect();
 timeClient.update();
 hh = timeClient.getHours();
 mm = timeClient.getMinutes();
 ss = timeClient.getSeconds();
 h= String(hh); 
 m= String(mm);
 h +" :" + m;
 
 entrysensor= !digitalRead(carEnter);
 s1 = !digitalRead(slot1);
 s2 = !digitalRead(slot2);

 newcount=count;
 newstr=str;
 
 
  
  
   if( digitalRead(s1) && digitalRead(s2) )   //1 ; all slot available
 {
  Serial.println("Welcome, Available: sLOT1, sLOT2");  //print slot1 and slo2  available
str=" SLOT 1,SLOT 2 AVAILABLE";
  digitalWrite(led1,HIGH);
  digitalWrite(led2,HIGH);
  digitalWrite(led4,LOW);
  if( !(digitalRead(entrysensor))){
      myservos.write(OPEN_ANGLE);
                                //increment count
      delay(3000);
      myservos.write(CLOSE_ANGLE);
      }
   count=0;
  
   
}

 if( !(digitalRead(s1)) && digitalRead(s2)  )  //2 ; slot2 available
  {
      Serial.println("Welcome, Available: sLOT2");
      str=" SLOT 2 AVAILABLE";
      digitalWrite(led1,LOW);
      digitalWrite(led2,HIGH);
      digitalWrite(led4,LOW);
      if( !(digitalRead(entrysensor))){
      myservos.write(OPEN_ANGLE);
                                //increment count
      delay(3000);
      myservos.write(CLOSE_ANGLE);
      }
       count=1;
     
     
  }

   if( digitalRead(s1) && !(digitalRead(s2)) )     //3 ; slot1  available
   {
     Serial.println("Welcome, Available: sLOT1 ");
     str=" SLOT 1 AVAILABLE";
      digitalWrite(led1,HIGH);
      digitalWrite(led2,LOW);
      digitalWrite(led4,LOW);
      delay(1000);
      if( !(digitalRead(entrysensor))){
      myservos.write(OPEN_ANGLE);
                                //increment count
      delay(3000);
      myservos.write(CLOSE_ANGLE);
      }
      count=1;
      
   
   }
 

    if( !(digitalRead(s1)) && !(digitalRead(s2))) //8  no slot available
     {
       Serial.println("Welcome, Parking Full");// No slot available
       count=2;
       str="PARKING FULL";
       digitalWrite(led1,LOW);
        digitalWrite(led2,LOW);
        digitalWrite(led4,HIGH);
        delay(1000);

        

     }
   r!=newstr)
    {
      if (! slotstatus.publish((char*) str.c_str())) {}
     Serial.print("slot status published");
    }
 
   
 
   
   
  
if(newcount != count){
     if (! CarsParked.publish(count)) {}
     Serial.print("count published");
    
  if (s1 == 1 && s1_occupied == false) {                     
        Serial.println("Occupied1 ");
        EntryTimeSlot1 =  h +" :" + m;
        //Serial.print("EntryTimeSlot1");
        //Serial.print(EntryTimeSlot1);
        
        s1_occupied = true;
        if (! EntrySlot1.publish((char*) EntryTimeSlot1.c_str())){}
    }
  if(s1 == 0 && s1_occupied == true) {
       Serial.println("Available1 ");
       ExitTimeSlot1 =  h +" :" + m;
       //Serial.print("ExitTimeSlot1");
       //Serial.print(ExitTimeSlot1);
     
       s1_occupied = false;
        if (! ExitSlot1.publish((char*) ExitTimeSlot1.c_str())){} 
}
  if (s2 == 1&& s2_occupied == false) {                     
      Serial.println("Occupied2 ");
      EntryTimeSlot2 =  h +" :" + m;
      //Serial.print("EntryTimeSlot2");
      //Serial.print(EntryTimeSlot2);
     
      s2_occupied = true;
      if (! EntrySlot2.publish((char*) EntryTimeSlot2.c_str())){}
    }
  if(s2 == 0 && s2_occupied == true) {
       Serial.println("Available2 ");
       ExitTimeSlot2 =  h +" :" + m;
       //Serial.print("ExitTimeSlot2");
       //Serial.print(ExitTimeSlot2);
       
       s2_occupied = false;
        if (! ExitSlot2.publish((char*) ExitTimeSlot2.c_str())){}
  }
}
  } 

  

void MQTT_connect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
}
 

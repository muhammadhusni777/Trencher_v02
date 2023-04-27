/*
mosquitto_pub -h 123.45.0.10 -p 1883 -t hoho -m *sudut servo*

wheels 
off max : 173
on max : 255

*/
#include <Wire.h>

//#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>



int wheel_left_fwd_pin = 12;
int wheel_right_fwd_pin = 11;

int wheel_left_rev_pin = 9;
int wheel_right_rev_pin = 8;

int wheel_speed_pin = 7;

int boom_up_pin = 5;
int boom_down_pin = 6;

String wheel_left_dir;
String wheel_right_dir;

//int dir_pin = 4;


int waterjet_pin = 4;

int val1;
int val2;

int wdt_pin = 3;
int wdt_count;

unsigned long time_now;
unsigned long time_elapsed;
unsigned long time_prev;

unsigned long time_send;
unsigned long time_send_prev;
int time_message = 1000;

//MechaQMC5883 qmc;
int azimuth =0;

int val;
// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xE6 }; //0x00, 0x80, 0xE1, 0x01, 0x01, 0x01
//byte mac[]    = {  0x00, 0x80, 0xE1, 0x01, 0x01, 0x01 };

IPAddress ip(123, 45, 0, 106);
IPAddress server(123, 45, 0, 10);

void callback(char* topic, byte* message, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
    
  }
  //val = messageTemp.toFloat();
 
  //Serial.println();
   if (String(topic) == "test_topic") {
   val = messageTemp.toInt(); 
   //Serial.println(val);
   }

  if (String(topic) == "boom") {

    
    if(messageTemp == "up"){    
      Serial.println("off");  
      digitalWrite(boom_up_pin, HIGH);
      digitalWrite(boom_down_pin, LOW);
    }
     else if(messageTemp == "down"){
      Serial.println("off");
      digitalWrite(boom_up_pin, LOW);
      digitalWrite(boom_down_pin, HIGH);
    }
    else if(messageTemp == "netral"){
      Serial.println("off");
      digitalWrite(boom_up_pin, LOW);
      digitalWrite(boom_down_pin, LOW);
    }

    
  }
   
  


  if (String(topic) == "wheel_dir") {

    
    if(messageTemp == "reverse"){    
      Serial.println("off");  
      wheel_left_dir = "rev";
      wheel_right_dir = "rev";
    }
     else if(messageTemp == "forward"){
      Serial.println("off");
      wheel_left_dir = "fwd";
      wheel_right_dir = "fwd";
    }
  }

  
  
  if (String(topic) == "wheel1") {
      val1 = messageTemp.toInt();  
      
    }

  if (String(topic) == "wheel2") {
      val2 = messageTemp.toInt(); 
    
    }

  if (String(topic) == "waterjet") {
      if(messageTemp.toInt() > 50){
        digitalWrite(waterjet_pin, HIGH);   
      } else {
        digitalWrite(waterjet_pin, LOW);   
      }
       
    }

   
    
  

 messageTemp ="";
}

EthernetClient ethClient;
PubSubClient client(ethClient);
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("CompassClient")) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("hoho");
      client.subscribe("test_topic");
      
      client.subscribe("wheel1");
      client.subscribe("wheel2");
      client.subscribe("wheel_dir");
      client.subscribe("boom");
      client.subscribe("waterjet");
      
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
      wdt_count = wdt_count + 1;
      if (wdt_count > 2){
        digitalWrite(wdt_pin, LOW);
      }
    }
  } 
}
void setup() {
  TCCR1B = TCCR1B&B11111000|B00000100;
  Wire.begin();
  Serial.begin(115200);
  Serial.println("booting....");
  //qmc.init();
  client.setServer(server, 1883);
  client.setCallback(callback);
  pinMode(wdt_pin, OUTPUT);
  digitalWrite(wdt_pin, HIGH);

  pinMode (wheel_left_fwd_pin, OUTPUT);
  pinMode (wheel_right_fwd_pin, OUTPUT);
  
  pinMode (wheel_left_rev_pin, OUTPUT);
  pinMode (wheel_right_rev_pin, OUTPUT);


  digitalWrite(wheel_right_fwd_pin, HIGH);
  digitalWrite(wheel_right_rev_pin, HIGH);
  digitalWrite(wheel_left_fwd_pin, HIGH);
  digitalWrite(wheel_left_rev_pin, HIGH);
  
  pinMode (boom_up_pin, OUTPUT);
  pinMode (boom_down_pin, OUTPUT);
//  pinMode (dir_pin, OUTPUT);

  pinMode(waterjet_pin, OUTPUT);
  
  
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  //delay(1500);
  //qmc.setMode(Mode_Continuous,ODR_200Hz,RNG_2G,OSR_256);
  
}

static char yaw_send[15];
void loop() {
  time_now = millis();
  
  if (!client.connected()) {
    reconnect();
  }

  if (wheel_left_dir == "rev"){
    analogWrite(wheel_left_rev_pin, 255);
    analogWrite(wheel_left_fwd_pin, 0);

    if (val1 > val2){
    analogWrite(wheel_speed_pin, val1);
  } else {
    analogWrite(wheel_speed_pin, val2);
  }
    
  }

  if (wheel_left_dir == "fwd"){
    analogWrite(wheel_left_rev_pin, 0);
    analogWrite(wheel_left_fwd_pin, 255);
  
    if (val1 > val2){
    analogWrite(wheel_speed_pin, val1);
  } else {
    analogWrite(wheel_speed_pin, val2);
  }
  }

  if (wheel_right_dir == "rev"){
    analogWrite(wheel_right_rev_pin, 255);
    analogWrite(wheel_right_fwd_pin, 0);
  
    if (val1 > val2){
    analogWrite(wheel_speed_pin, val1);
  } else {
    analogWrite(wheel_speed_pin, val2);
  }
  
  }

  if (wheel_right_dir == "fwd"){
    analogWrite(wheel_right_rev_pin, 0);
    analogWrite(wheel_right_fwd_pin, 255);
  
    if (val1 > val2){
    analogWrite(wheel_speed_pin, val1);
  } else {
    analogWrite(wheel_speed_pin, val2);
  }
  }

  
  
  /*
  analogWrite(wheel_left_pin, wheel_left_speed);
  analogWrite(wheel_right_pin, wheel_right_speed);
  */
  

  time_send = millis() - time_send_prev;
  if (time_send > time_message){
 
 
  client.publish("Yaw",dtostrf(azimuth,6,3,yaw_send)); // kirim data
  //client.publish("Yaw, dtostrf(100));
  Serial.println();
  time_send_prev = millis();
  }
  
  
  client.loop();
  time_elapsed = time_now - time_prev;
  time_prev = time_now;


  
}  


#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#include <QMC5883LCompass.h>

QMC5883LCompass compass;





#include "I2Cdev.h"
#include "MPU6050.h"


#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

int roll_raw;
int pitch_raw;

#define OUTPUT_READABLE_ACCELGYRO



int16_t adc_left;
int16_t adc_middle;
int16_t adc_right;




int compass_raw;
String wheel_dir;
String readString;


int depth;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xE0 };
IPAddress ip(123, 45, 0, 101);
IPAddress server(123, 45, 0, 100);


void callback(char* topic, byte* message, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  String messageTemp;
  for (int i=0;i<length;i++) {

    //Serial.print((char)payload[i]);
   messageTemp += (char)message[i];
  }

  

  if (String(topic) == "wheel_dir") {
      wheel_dir = messageTemp;  
    }
  
  }
  

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() { 
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
   
    if (client.connect("Trencher")) {
      Serial.println("connected");
      client.subscribe("wheel_dir");
      client.subscribe("wheel1");
      client.subscribe("wheel2");
      client.subscribe("propeller1");
      client.subscribe("propeller2");
      client.subscribe("propeller3");
      client.subscribe("propeller4");
    
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  } 
}




void setup() {
  Serial.begin(9600);

  

  accelgyro.initialize();
  
  compass.init();
  

  
  delay(5000);                          // Wait for a while

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);


}

static char compass_send[13];
static char sensor_left_send[13];
static char sensor_right_send[13];
static char sensor_middle_send[13];
static char pitch_send[13];
static char roll_send[13];
static char depth_send[13];



void loop() {

  if (!client.connected()) {
    reconnect();
  }

  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  #ifdef OUTPUT_READABLE_ACCELGYRO
       // display tab-separated accel/gyro x/y/z values
      //  Serial.print("a/g:\t");
      //  Serial.print(ax); Serial.print("\t");
       // Serial.print(ay); Serial.print("\t");
       // Serial.print(az); Serial.print("\t");
      //  Serial.print(gx); Serial.print("\t");
      //  Serial.print(gy); Serial.print("\t");
      //  Serial.println(gz);
      //  Serial.println("");
  #endif

   #ifdef OUTPUT_BINARY_ACCELGYRO
        Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
        Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
        Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
        Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
        Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
        Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
    #endif

  compass.read();
  compass_raw = compass.getAzimuth();
  
  
  roll_raw = map(az, -16000, 16000, -180, 180);
  pitch_raw = map(ay, -16000, 16000, -180, 180);

  if (wheel_dir == "forward"){
    
  }

  if (wheel_dir == "reverse"){
    
  }

  
  //delay(15);                            // Wait for a while
  
  depth = analogRead(A0);
  time_send = millis() - prev_time_send;
  if(time_send > 200){
  
  client.publish("yaw",dtostrf(compass_raw,6,5,compass_send));
  client.publish("roll",dtostrf(roll_raw,6,5,roll_send));
  client.publish("pitch",dtostrf(pitch_raw,6,5,pitch_send));
  client.publish("depth",dtostrf(depth,6,5,depth_send));
  prev_time_send = millis();
  }
  client.loop();
}


#include <DFRobot_WT61PC.h>
#include <SoftwareSerial.h>

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <avr/wdt.h>

//Use software serial port RX：10，TX：11
SoftwareSerial mySerial(9, 11);

DFRobot_WT61PC sensor(&mySerial);

EthernetClient ethClient;
PubSubClient client(ethClient);

int x;
int y;
int z;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xE1 };
IPAddress ip(123, 45, 0, 107);
IPAddress server(123, 45, 0, 10);

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String messageTemp;
  
  for (int i=0;i<length;i++) {

    //Serial.print((char)payload[i]);
   messageTemp += (char)message[i];
  }

}


void reconnect() { 
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("gyroscope")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("Steering_DP 4");
    
    } else {
      Serial.print("fail, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  } 
}



void setup()
{
  //Use Serial as debugging serial port 
  Serial.begin(115200);
  client.setServer(server, 1883);
  client.setCallback(callback);

  
  Ethernet.begin(mac, ip);
  
  //Use software serial port mySerial as communication seiral port 
  mySerial.begin(9600);
  //Revise the data output data frequncy of sensor FREQUENCY_0_1HZ for 0.1Hz, FREQUENCY_0_5HZ for 0.5Hz, FREQUENCY_1HZ for 1Hz, FREQUENCY_2HZ for 2Hz, 
  //                        FREQUENCY_5HZ for 5Hz, FREQUENCY_10HZ for 10Hz, FREQUENCY_20HZ for 20Hz, FREQUENCY_50HZ for 50Hz, 
  //                        FREQUENCY_100HZ for 100Hz, FREQUENCY_125HZ for 125Hz, FREQUENCY_200HZ for 200Hz.
  sensor.modifyFrequency(FREQUENCY_10HZ);
}

static char x_send[10];
static char y_send[10];
static char z_send[10];

void loop()
{
  if (sensor.available()) {

    x = sensor.Angle.X;
    y = sensor.Angle.Y;
    z = sensor.Angle.Z;
    Serial.print("Acc\t"); Serial.print(sensor.Acc.X); Serial.print("\t"); Serial.print(sensor.Acc.Y); Serial.print("\t"); Serial.println(sensor.Acc.Z); //acceleration information of X,Y,Z
    Serial.print("Gyro\t"); Serial.print(sensor.Gyro.X); Serial.print("\t"); Serial.print(sensor.Gyro.Y); Serial.print("\t"); Serial.println(sensor.Gyro.Z); //angular velocity information of X,Y,Z
    Serial.print("Angle\t"); Serial.print(x); Serial.print("\t"); Serial.print(y); Serial.print("\t"); Serial.println(z); //angle information of X, Y, Z 
    Serial.println(" ");
  
  
    client.publish("blade_angle", dtostrf(x,6,0,x_send));
  
  }
}

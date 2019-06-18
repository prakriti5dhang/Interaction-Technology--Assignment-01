#include <SPI.h>
#include <Ethernet.h> //library for ethernet
#include <PubSubClient.h> // library for publish Subscribe

#include <DHT.h> //library for DHT sensor

const int ledPinG = 13; // green led attached to digital pin 13 of the arduino board
const int ledPinY = 12; //yellow led attached to digital pin 12 of the arduino board
const int ledPinR = 11; //red led attached to digital pin 11 of the arduino board

const int ldrPin = A0; //LDR sensor attached to analog pin A0 of the arduino board

const int ThermistorPin = A1; //thermistor input attached to analog pin A1 of the arduino board
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

#define DHT22_PIN 7 //DHT sensor attached to digital pin 7 to the arduino board
#define DHTTYPE DHT22 //DHT sensor type
DHT dht( DHT22_PIN, DHTTYPE); // DHT pinnumber and type
unsigned long readTime;

const int soundpin = A2; //Sound sensor attached to analog pin A2 of the arduino board
const int threshold = 523; // fixed threshold for sound sensor

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
const char* server = "m16.cloudmqtt.com"; //MQTT server
const char* username = "yifzrfid"; 
const char* password = "UQW0Ni-cpe9W";

char message_buff[100]; // this buffers our incoming messages so we can do something on certain commands
char buffer[10];
EthernetClient ethClient;
PubSubClient client(ethClient);



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  int i=0;
  for (i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    message_buff[i] = payload[i];
  }
 /* message_buff[i] = '\0';
  String msgString = String(message_buff);
  if (msgString.equals("OFF")) {
   // client.publish("openhab/himitsu/command","acknowedging OFF");
  }
  else if(msgString.equals("ON")){
   // client.publish("openhab/himitsu/command","acknowedging ON");
  }*/
  Serial.println();
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
   
 
    // Attempt to connect
    if (client.connect("arduinoClient",username, password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Greetings","Hello welcome");
      // ... and resubscribe
      client.subscribe("Welcome");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(15000);
    }
  }
}


void setup() {

Serial.begin(9600);

client.setServer(server, 17105);
  client.setCallback(callback);

  Ethernet.begin(mac);
  dht.begin();
  
  // Allow the hardware to sort itself out
  //delay(1500);
  Serial.println(Ethernet.localIP());
  readTime = 0;

pinMode(ledPinG, OUTPUT); // LED green output
pinMode(ldrPin, INPUT); // input for the LDR sensor

pinMode(ledPinY, OUTPUT); //LED yellow output
  //dht.begin();

 
pinMode(ledPinR, OUTPUT); //LED red output
pinMode(soundpin, INPUT); // input for the Sound sensor

 
 
}

void loop() {

   if (!client.connected()) {
    reconnect();
  }

  client.loop();

  if(millis() > readTime+200000){
/* code for LDR sensor starts */
//https://create.arduino.cc/projecthub/tarantula3/using-an-ldr-sensor-with-arduino-807b1c


  
//delay(1000);


/* code for DHT sensor starts */
//https://create.arduino.cc/projecthub/mafzal/temperature-monitoring-with-dht22-arduino-15b013
//https://www.instructables.com/id/How-to-use-DHT-22-sensor-Arduino-Tutorial/

  float h =dht.readHumidity(); //reads humidity
  float t =dht.readTemperature(); //reads the temperature in Celicus
  float f = dht.readTemperature(true); //reads the temperature in Fahrenit
  
  Serial.println("..............");
  Serial.println("... DHT Sensor ...");
  Serial.println("..............");
  dtostrf(t,0, 0, buffer);
  client.publish("Temperature",buffer); //publishes data to the MQTT
  dtostrf(h,0, 0, buffer);
  client.publish("Humidity",buffer); //publishes data to the MQTT
  
  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.println(" *C");
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.println("%");
  Serial.print("Fahrenit temperature = ");
  Serial.print(f);
  Serial.println(" *F");
  
  
  //char buffer[10];
  
  
  

if(f>60)
{
  digitalWrite(ledPinY, HIGH);
  Serial.println("Temperature is high: Yellow LED is on");
  client.publish("Stauts of the YELLOW LED","ON");

  //delay(1000);
}
else
{
  digitalWrite(ledPinY, LOW);
  Serial.println("Temperature is low: Yellow LED is off");
  client.publish("Stauts of the YELLOW LED","OFF");

}

/*code for DHT sensor ends */

 delay(1000);
 
 /* code for sound sensor starts */
 //Arduino project handbook Vol2
 
 Serial.println("..............");
Serial.println("... SOUND SENSOR ...");
Serial.println("..............");

 int noise = analogRead(soundpin);
Serial.println(noise);
dtostrf(noise,0, 0, buffer);
client.publish("Sound Status",buffer); //publishes data to the MQTT



if (noise >= threshold)
{
  digitalWrite(ledPinR, HIGH);
  Serial.println("More sound: Red LED is on");
  client.publish("Stauts of the RED LED","ON");

  delay(1000);
}

else{
  digitalWrite(ledPinR, LOW);
  Serial.println("No sound: Red LED is off");
  client.publish("Stauts of the RED LED","OFF");

}
 /* code for sound sensor ends */
 delay(1000);
/* code for LDR sensor starts */
 
 Serial.println("..............");
Serial.println("... Light Sensor ...");
Serial.println("..............");

int ldrStatus = analogRead(ldrPin);
Serial.print("LDR Status: ");
Serial.println(ldrStatus);

dtostrf(ldrStatus,0, 0, buffer);
client.publish("LDR status",buffer); //publishes data to the MQTT
//delay(1000);
if (ldrStatus <= 750) {

digitalWrite(ledPinG, HIGH); 

Serial.println("No light: Turns on the Green LED  ");
client.publish("Stauts of the Green LED","ON");

//Serial.println(ldrStatus);

} else {

digitalWrite(ledPinG, LOW);
Serial.println("More light: Turns off the Green LED ");
client.publish("Stauts of the Green LED","OFF");
//Serial.println(ldrStatus);

}

/* code for LDR sensor ends */

delay(1000);

 /*code for Thermistor sensor starts*/
//http://www.circuitbasics.com/arduino-thermistor-temperature-sensor-tutorial/

Serial.println("..............");
Serial.println("... Thermistor Sensor ...");
Serial.println("..............");

 Vo = analogRead(ThermistorPin);
  Serial.println(Vo);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15;
  Tf = (Tc * 9.0)/ 5.0 + 32.0; 

  //char buffer[10];
  dtostrf(Tf,0, 0, buffer);

  Serial.print("Temperature from Thermistor: "); 
  Serial.print(Tf);
 client.publish("Temperature in Fahrenit from Thermistor",buffer); //publishes data to the MQTT
  Serial.print(" F; ");
  Serial.print(Tc);
  dtostrf(Tc,0, 0, buffer);
  client.publish("Temperature in Celcius from Thermistor",buffer); //publishes data to the MQTT
  Serial.println(" C");   

  /*code for Thermistor sensor ends */
  }

}

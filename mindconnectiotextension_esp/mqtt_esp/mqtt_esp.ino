#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11

// DHT Sensor
uint8_t DHTPin = D1;

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

String Temperature;
String Humidity;

const char *ssid = "i40tk"; 
const char *password = "PbKbTtKa5";

const char *mqtt_broker = "mciotextension.eu1.mindsphere.io";
const char *topic = "s/us";
const char *mqtt_username = "bmedev/mindconnectuser";
const char *mqtt_password = "Kiskacsa0";
//set mqtt clientID
const char *clientid="2e04f278-8e04-48a5-821e-69f26448af38";

const int mqtt_port = 1883;

IPAddress local_IP(192, 168, 33, 150);
// Set your Gateway IP address
IPAddress gateway(192, 168, 33, 254);
IPAddress subnet(255, 255, 255, 0);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
 // Set software serial baud to 115200;
 Serial.begin(115200);
 Serial.flush();

 pinMode(DHTPin, INPUT);
 dht.begin();
 // connecting to a WiFi network
 if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
 }
 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);

 while (!client.connected()) {
     Serial.println("Connecting to public mqtt broker.....");
     if (client.connect(clientid, mqtt_username, mqtt_password)) {
         Serial.println("Public mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
}

void reconnect(){
  while (!client.connected()) {
    if (client.connect(clientid, mqtt_username, mqtt_password)) {
           Serial.println("Public mqtt broker connected");
    }
    Serial.println("Working on it");
  }
  Temperature = String(dht.readTemperature()); // Gets the values of the temperature
  Humidity = String(dht.readHumidity()); // Gets the values of the humidity
  String temp="200,measurement,Temperature,"+Temperature+"\n"+"200,measurement,Humidity,"+Humidity;
  char payload[temp.length()+1];
  temp.toCharArray(payload,temp.length()+1);
  Serial.println("Temp:"+Temperature);
  Serial.println("Humi:"+Humidity);
  client.publish(topic, payload);
  client.subscribe(topic);
  delay(1000);
}

void loop() {
  reconnect();
  client.loop();
}

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

const char* ssid = "i40tk";
const char* password = "PbKbTtKa5"; 

const char* mqtt_Server = "mciotextension.eu1.mindsphere.io";
const int mqtt_Port = 1883;
const char* mqtt_user = "bmedev/mindconnectuser";
const char* mqtt_password = "Kiskacsa0";

WiFiClient espClient;
PubSubClient client(espClient);

/*void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}*/
void sendMessage(){
  Temperature = String(dht.readTemperature()); // Gets the values of the temperature
  Humidity = String(dht.readHumidity()); // Gets the values of the humidity
  String temp="200,measurement,Temperature,"+Temperature+"\n"+"200,measurement,Humidity,"+Humidity;
  char payload[temp.length()+1];
  temp.toCharArray(payload,temp.length()+1);
  Serial.println("Temp:"+Temperature);
  Serial.println("Humi:"+Humidity);
  client.publish("s/us", payload);
  delay(1000);
}

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "2e04f278-8e04-48a5-821e-69f26448af38";
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
    Serial.println("connected");
    client.subscribe("s/us");
    } else {
      Serial.print("failed, rc= ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
  sendMessage();
}


void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi network");

  client.setServer(mqtt_Server, mqtt_Port);
  //client.setCallback(callback);
  reconnect();
  //client.subscribe("s/us"); //I also tryed s/ds

}

void loop() {
  client.loop();
  delay(1000);
  reconnect();
} 

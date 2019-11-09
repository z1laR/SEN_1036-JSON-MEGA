#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "RestClient.h"

#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

PulseSensorPlayground pulseSensor;

#define IP "192.168.0.101"
#define PORT 3000

String postPulsoCardiaco = "";

const int PulseWire = 0;
const int LED = 4;
int Threshold = 550;
                               
RestClient client = RestClient(IP, PORT);

void setup()
{
  Serial.begin(9600);

  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED);
  pulseSensor.setThreshold(Threshold);

  Serial.println("Conectado a la red:");
  client.dhcp();

  Serial.print("IP de Placa Ethernet Shield:");
  Serial.println(Ethernet.localIP());
  
  Serial.println("Listo :D!");

  if (pulseSensor.begin())
  {
    Serial.println("Sensor de Pulso Caridaco SEN_1036 -----> Listo");
  }
}

void loop()
{
  int LPM = pulseSensor.getBeatsPerMinute();

  if (pulseSensor.sawStartOfBeat())
  {
    Serial.println("");
    Serial.println("♥  Se a detectado una pulsación ♥ ");
    Serial.print("LPM: ");
    Serial.println(LPM);
    Serial.println("");

    client.setHeader("Content-Type: application/json");

    StaticJsonBuffer<200> jsonBuffer;
    char json[256];  
    JsonObject& root = jsonBuffer.createObject();
    root["lpm"] = LPM;
    Serial.println("Estructura JSON:");
    root.printTo(json, sizeof(json));
    Serial.println("");
    Serial.println(json);

    int statusCode = client.post("http://192.168.0.101:3000/api/pulso-cardiaco", json, &postPulsoCardiaco);

    Serial.print("Repuesta del servidor: ");
    Serial.println(statusCode);
    Serial.println("");
    Serial.println("Respuesta MongoDB -> " +postPulsoCardiaco);
    /*Serial.print("postPulsoCardiaco body from server: ");
    Serial.println(postPulsoCardiaco);*/
    Serial.println("");

    postPulsoCardiaco = "";
  }
  
  delay(20);
}

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "../lib/CapteurSonore.h"
#include "../lib/ServeurHTTP.h"

const char* ssid = "Ornella KOM";
const char* password = "Ornella2023";
const int pinCapteurSonore = 32;

CapteurSonore capteur(pinCapteurSonore);
ServeurHTTP serveur;

AsyncWebServer server(80);

void setup()
{
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi.....");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request)
            { serveur.handleRequest(request, capteur); });

  server.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
}

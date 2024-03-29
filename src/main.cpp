#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "../lib/CapteurSonore.h"
#include "../lib/ServeurHTTP.h"
#include <LittleFS.h>

const char *ssid = "Ariane";
const char *password = "canada2023";
const int pinCapteurSonore = 32;
const int seuilSonore = 500; // Seuil de déclenchement du niveau sonore

const int pinLedVerte = 27;
const int pinLedRouge = 26;
const int pinHautParleur = 33;

CapteurSonore capteur(pinCapteurSonore);
ServeurHTTP serveur;

AsyncWebServer server(80);

void initSPIFFS()
{
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS"); }
  else{
    Serial.println("LittleFS mounted successfully");
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(pinLedVerte, OUTPUT);
  pinMode(pinLedRouge, OUTPUT);
  pinMode(pinHautParleur, OUTPUT);

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

  initSPIFFS();
  Serial.println(LittleFS.exists("/index.html"));
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              request->send(LittleFS, "/index.html", "text/html");
            });

           server.serveStatic("/", LittleFS, "/");

  server.begin();
}

void loop()
{
  int niveauSonore = capteur.lireValeur();

  if (niveauSonore > seuilSonore)
  {
    digitalWrite(pinLedRouge, HIGH);
    digitalWrite(pinLedVerte, LOW);
    tone(pinHautParleur, 1000); // Jouer un son
  }
  else
  {
    digitalWrite(pinLedRouge, LOW);
    digitalWrite(pinLedVerte, HIGH);
    noTone(pinHautParleur); // Arrêter le son
  }

  // Mise à jour des données toutes les 5 secondes
  delay(5000);
}

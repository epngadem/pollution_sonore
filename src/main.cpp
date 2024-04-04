#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "../lib/CapteurSonore.h"
#include "../lib/ServeurHTTP.h"
#include "SPIFFS.h"

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

// Replaces placeholder with LED value
String processor(const String &var)
{
    if (var == "SCORE")
    {
        return String(capteur.lireValeur());
    }
    return String();
}

void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Une erreur s'est produite lors du montage de SPIFFS");
    }
    else
    {
        Serial.println("SPIFFS monté avec succès");
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(pinCapteurSonore, INPUT);
    pinMode(pinLedVerte, OUTPUT);
    pinMode(pinLedRouge, OUTPUT);
    pinMode(pinHautParleur, OUTPUT);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    initSPIFFS();
    Serial.println(WiFi.localIP());

    // Route pour servir le contenu de index.html lors de l'accès à "/"

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html", false, processor); });

    server.serveStatic("/", SPIFFS, "/");

    // Route pour renvoyer les données du capteur sous forme de JSON
    server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    int noiseLevel = capteur.lireValeur();
    String jsonResponse = "{";
    jsonResponse += "\"niveauSonore\":" + String(noiseLevel);
    jsonResponse += "}";

    // Réponse HTTP envoyée au client
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonResponse);
    response->addHeader("Access-Control-Allow-Origin", "*"); 
    response->addHeader("Access-Control-Allow-Methods", "GET");
    request->send(response); });

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

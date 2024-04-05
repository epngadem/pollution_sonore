/*
  Titre      : systeme de Surveillance sonore de maniere Synchrone (Communication)
  Auteur     : Princesse Ornella NGADEM KOM
  Date       : 05/04/2024
  Description: Ce code nous permet de faire des collectes de donnees du niveau sonore au CCNB dans ses differents locaux tout en alertant les les professeurs
  en cas d extreme bruits grace a nos actionneurs quon a defini et cela  pour retablir lordre dans ses locaux.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "../lib/CapteurSonore.h"
#include "../lib/ServeurHTTP.h"
#include "../lib/LedVerteActuator.h"
 #include "../lib/LedRougeActuator.h"
#include "../lib/SpeakerActuator.h"
#include "SPIFFS.h"

const char *ssid = "Ariane";
const char *password = "canada2023";

const int pinCapteurSonore = 32; // Broche du capteur sonore
const int seuilSonore = 500;     // Seuil de déclenchement du niveau sonore

const int pinLedVerte = 27;    // Broche de la LED verte
const int pinLedRouge = 26;    // Broche de la LED rouge
const int pinHautParleur = 33; // Broche du haut-parleur

CapteurSonore capteur(pinCapteurSonore);
ServeurHTTP serveur;
SpeakerActuator hautParleur(pinHautParleur); // Objet pour le contrôle du haut-parleur
LedVerteActuator ledVerte(pinLedVerte);           // Objet pour le contrôle de la LED verte
LedRougeActuator ledRouge(pinLedRouge);           // Objet pour le contrôle de la LED rouge

String niveauSonoreString;
int niveauSonore;

AsyncWebServer server(80);

String processor(const String &var) {
    if (var == "SCORE") {
        return niveauSonoreString;
    }
    if (var == "NIVEAU_SONORE") {
        Serial.println(' var : ' + String(niveauSonore));
        return String(niveauSonore);
    }
    return String();
}

void initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Une erreur s'est produite lors du montage de SPIFFS");
    } else {
        Serial.println("SPIFFS monté avec succès");
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(pinCapteurSonore, INPUT);
    pinMode(pinLedVerte, OUTPUT);
    pinMode(pinLedRouge, OUTPUT);
    pinMode(pinHautParleur, OUTPUT);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    initSPIFFS();
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html", false, processor);
    });

    server.serveStatic("/", SPIFFS, "/");

    server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        int noiseLevel = capteur.lireValeur();
        String jsonResponse = "{";
        jsonResponse += "\"niveauSonore\":" + String(noiseLevel);
        jsonResponse += "}";

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonResponse);
        response->addHeader("Access-Control-Allow-Origin", "*");
        response->addHeader("Access-Control-Allow-Methods", "GET");
        request->send(response);
    });

    server.begin();
}

void loop() {
    niveauSonore = capteur.lireValeur();

    if (niveauSonore > seuilSonore) {
        ledRouge.allumer();
        ledVerte.eteindre();
        hautParleur.jouerSon(1000);
    } else {
        ledRouge.eteindre();
        ledVerte.allumer();
        hautParleur.arreterSon();
    }

    niveauSonoreString = String(niveauSonore);
    delay(5000);
}


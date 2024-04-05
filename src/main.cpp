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
#include "../lib/CapteurSonore.h"// Inclusion de la classe pour mon capteur sonore 
#include "../lib/ServeurHTTP.h"// Inclusion de la classe serveur pour ma communication
#include "../lib/LedVerteActuator.h" // Inclusion de la classe pour contrôler la LED verte
#include "../lib/LedRougeActuator.h" // Inclusion de la classe pour contrôler la LED rouge
#include "../lib/SpeakerActuator.h"// Inclusion de la classe pour mon haut parleur 
#include "SPIFFS.h"

// Définition des constantes pour le réseau WiFi
const char *ssid = "UNIFI_IDO1";
const char *password = "41Bidules!";

// Broches pour les différents composants
const int pinCapteurSonore = 32; // Broche du capteur sonore
const int seuilSonore = 500;     // Seuil de déclenchement du niveau sonore

const int pinLedVerte = 27;    // Broche de la LED verte
const int pinLedRouge = 26;    // Broche de la LED rouge
const int pinHautParleur = 33; // Broche du haut-parleur

// Instanciation des objets pour les différents composants
CapteurSonore capteur(pinCapteurSonore); // Crée un objet CapteurSonore avec le numéro de broche
ServeurHTTP serveur;
SpeakerActuator hautParleur(pinHautParleur); //on a Créer un objet SpeakerActuator pour le haut-parleur
LedVerteActuator ledVerte(pinLedVerte);      // on a Créer un objet LedVerteActuator pour le contrôle de la LED verte
LedRougeActuator ledRouge(pinLedRouge);      //on a Créer un objet LedRougeActuator pour le contrôle de la LED rouge

// Variables pour le traitement des données
String niveauSonoreString; // Variable pour stocker le niveau sonore sous forme de chaîne
int niveauSonore;          // Variable pour stocker le niveau sonore sous forme d'entier

AsyncWebServer server(80); // Crée un serveur web asynchrone sur le port 80

// Fonction de traitement des variables de remplacement pour la page web
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

// Initialisation du système de fichiers SPIFFS
void initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Une erreur s'est produite lors du montage de SPIFFS");
    } else {
        Serial.println("SPIFFS monté avec succès");
    }
}

void setup() {
    Serial.begin(9600); // Initialisation de la communication série à 9600 bauds

    // Configuration des broches des différents composants
    pinMode(pinCapteurSonore, INPUT);
    pinMode(pinLedVerte, OUTPUT);
    pinMode(pinLedRouge, OUTPUT);
    pinMode(pinHautParleur, OUTPUT);

    WiFi.begin(ssid, password); // Démarrage de la connexion WiFi avec les identifiants fournis

    while (WiFi.status() != WL_CONNECTED) { // Attente de connexion WiFi
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    initSPIFFS(); // Initialisation du système de fichiers SPIFFS
    Serial.println(WiFi.localIP()); // Affichage de l'adresse IP locale obtenue par WiFi

    // Route pour servir le contenu de index.html lors de l'accès à "/"
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html", false, processor);
    });

    server.serveStatic("/", SPIFFS, "/"); // Sert les fichiers statiques depuis mon SPIFFS

    // Route pour renvoyer les données du capteur sous forme de JSON
    server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        int noiseLevel = capteur.lireValeur(); // Lit la valeur du capteur sonore
        String jsonResponse = "{";
        jsonResponse += "\"niveauSonore\":" + String(noiseLevel);
        jsonResponse += "}";

        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonResponse);
        response->addHeader("Access-Control-Allow-Origin", "*");
        response->addHeader("Access-Control-Allow-Methods", "GET");
        request->send(response);
    });

    server.begin(); // Démarre le serveur web
}

void loop() {
    niveauSonore = capteur.lireValeur(); // Lit la valeur du capteur sonore

    // Vérification du niveau sonore pour agir en conséquence
    if (niveauSonore > seuilSonore) {
        ledRouge.allumer();     // Allume la LED rouge
        ledVerte.eteindre();    // Éteint la LED verte
        hautParleur.jouerSon(1000); // Joue un son
    } else {
        ledRouge.eteindre();    // Éteint la LED rouge
        ledVerte.allumer();     // Allume la LED verte
        hautParleur.arreterSon(); // Arrête le son
    }

    niveauSonoreString = String(niveauSonore); // Convertit le niveau sonore en chaîne de caractères

    // Mise à jour des données toutes les 5 secondes
    delay(5000);
}


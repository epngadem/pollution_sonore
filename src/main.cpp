/*
  Titre      : systeme de Surveillance sonore de maniere Synchrone (Communication)
  Auteur     : Princesse Ornella NGADEM KOM
  Date       : 05/04/2024
  Description: Ce code nous permet de faire des collectes de donnees du niveau sonore au CCNB dans ses differents locaux tout en alertant les les professeurs
  en cas d extreme bruits grace a nos actionneurs quon a defini et cela pour retablir lordre dans ses locaux.
*/



#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "../lib/CapteurSonore.h"
#include "../lib/ServeurHTTP.h"
#include "SPIFFS.h"

const char *ssid = "Ariane";
const char *password = "canada2023";

const int pinCapteurSonore = 32; // Broche du capteur sonore
const int seuilSonore = 500;     // Seuil de déclenchement du niveau sonore

const int pinLedVerte = 27;    // Broche de la LED verte
const int pinLedRouge = 26;    // Broche de la LED rouge
const int pinHautParleur = 33; // Broche du haut-parleur

CapteurSonore capteur(pinCapteurSonore); // on a créer un objet CapteurSonore avec le numéro de broche
ServeurHTTP serveur;//on a  créer un objet ServeurHTTP

String niveauSonoreString;// Variable pour stocker le niveau sonore sous forme de chaîne
int niveauSonore;// Variable pour stocker le niveau sonore sous forme d'entier

AsyncWebServer server(80);// on a  créer un serveur web asynchrone sur le port 80


// Fonction de traitement des variables de remplacement pour la page web
String processor(const String &var)
{
    if (var == "SCORE")// jai creer un premiere variable %SCORE% qui se trouve sur ma page 
    {
        return niveauSonoreString;
    }
    if (var == "NIVEAU_SONORE")// jai creer ici une seconde variable %NIVEAU SONORE% qui me permettra de capturer ma donneee sonore sur le serveur
    {
        Serial.println(' var : ' + String(niveauSonore));
        return String(niveauSonore);
    }
    return String();
}
// Initialisation du système de fichiers SPIFFS
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
// Configuration initiale du programme
void setup()
{
    Serial.begin(9600);// Initialisation de la communication série à 9600 bauds

    pinMode(pinCapteurSonore, INPUT);// Configuration de la broche du capteur sonore en entrée
    pinMode(pinLedVerte, OUTPUT);// Configuration de la broche de la LED verte en sortie
    pinMode(pinLedRouge, OUTPUT);// Configuration de la broche de la LED rouge en sortie
    pinMode(pinHautParleur, OUTPUT);// Configuration de la broche du haut-parleur en sortie

    WiFi.begin(ssid, password);// on démarre la connexion WiFi avec les identifiants fournis

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    initSPIFFS(); // Initialisation du système de fichiers SPIFFS
    Serial.println(WiFi.localIP()); // on Affiche l'adresse IP locale obtenue par WiFi

    // Route pour servir le contenu de index.html lors de l'accès à "/"

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html", false, processor); });

    server.serveStatic("/", SPIFFS, "/"); // Sert les fichiers statiques depuis SPIFFS


    // Route pour renvoyer les données du capteur sous forme de JSON
    server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    int noiseLevel = capteur.lireValeur();// Lit la valeur du capteur sonore
    String jsonResponse = "{";
    jsonResponse += "\"niveauSonore\":" + String(noiseLevel);
    jsonResponse += "}";

    // Réponse HTTP envoyée au client
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonResponse);
    response->addHeader("Access-Control-Allow-Origin", "*"); 
    response->addHeader("Access-Control-Allow-Methods", "GET");
    request->send(response); });

    server.begin();// Démarre le serveur web
}
// Boucle principale du programme
void loop()
{
    niveauSonore = capteur.lireValeur();; // Lit la valeur du capteur sonore

    if (niveauSonore > seuilSonore)// Vérifie si le niveau sonore dépasse le seuil
    {
        digitalWrite(pinLedRouge, HIGH);// Allume la LED rouge
        digitalWrite(pinLedVerte, LOW);// Éteint la LED verte
        tone(pinHautParleur, 1000); // Jouer un son
    }
    else// Si le niveau sonore est inférieur ou égal au seuil
    {
        digitalWrite(pinLedRouge, LOW);// Éteint la LED rouge
        digitalWrite(pinLedVerte, HIGH);// Allume la LED verte
        noTone(pinHautParleur); // Arrêter le son
    }
    niveauSonoreString = String(niveauSonore);// Convertit le niveau sonore en chaîne de caractères
    // Mise à jour des données toutes les 5 secondes
    delay(5000);
}

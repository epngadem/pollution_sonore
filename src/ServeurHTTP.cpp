#include "../lib/ServeurHTTP.h"

ServeurHTTP::ServeurHTTP() {}

void ServeurHTTP::handleRequest(AsyncWebServerRequest *request, CapteurSonore &capteur)
{
    int noiseLevel = capteur.lireValeur();
    String jsonResponse = "{";
    jsonResponse += "\"niveauSonore\":" + String(noiseLevel);
    jsonResponse += "}";

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonResponse);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET");
    request->send(response);
}
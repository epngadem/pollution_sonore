#ifndef SERVEUR_HTTP_H
#define SERVEUR_HTTP_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "CapteurSonore.h"

class ServeurHTTP
{
public:
    ServeurHTTP();
    void handleRequest(AsyncWebServerRequest *request, CapteurSonore &capteur);
};

#endif

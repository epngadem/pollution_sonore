#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "../lib/CapteurSonore.h"
#include "../lib/ServeurHTTP.h"

const char* ssid = "Ornella KOM";
const char* password = "Ornella2023";

const int pinCapteurSonore = 32;
const int seuilSonore = 500; // Seuil de déclenchement du niveau sonore

const int pinLedVerte = 27;
const int pinLedRouge = 26;
const int pinHautParleur = 33;

CapteurSonore capteur(pinCapteurSonore);
ServeurHTTP serveur;

AsyncWebServer server(80);

const char* indexHtml = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Surveillance Sonore en Temps Réel</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
        }

        .container {
            max-width: 800px;
            margin: auto;
            padding: 20px;
        }

        #graphiqueSonore {
            width: 100%;
            max-height: 400px;
        }

        .presentation {
            background-color: #f4f4f4;
            padding: 20px;
            border-radius: 5px;
            margin-bottom: 20px;
        }

        h1,
        h2 {
            text-align: center;
        }

        .alert {
            margin-top: 20px;
        }

        .led {
            width: 50px;
            height: 50px;
            border-radius: 50%;
            margin: 10px;
            display: inline-block;
        }

        .led-verte {
            background-color: green;
        }

        .led-rouge {
            background-color: red;
        }

        .led-allumee {
            box-shadow: 0 0 10px 5px yellow; /* Effet de lueur pour indiquer que la LED est allumée */
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Surveillance Sonore en Temps Réel</h1>
        <div class="presentation">
            <h2>Présentation du Projet</h2>
            <p>Ce projet vise à surveiller le niveau sonore ambiant en temps réel. Un capteur sonore est utilisé pour capturer les données sonores, qui sont ensuite envoyées à une page web via une connexion Internet. Les données sont affichées sous forme de graphique pour une visualisation facile et une analyse rapide.</p>
        </div>
        <canvas id="graphiqueSonore"></canvas>
        <div id="donneesCapteur" class="alert"></div> <!-- Div pour afficher les données renvoyées par Arduino -->
        <div class="led led-verte" id="ledVerte"></div>
        <div class="led led-rouge" id="ledRouge"></div>
    </div>

    <script>
        var graphique;

        // Configuration initiale du graphique
        function configurerGraphique() {
            var ctx = document.getElementById('graphiqueSonore').getContext('2d');
            graphique = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        label: 'Niveau Sonore (dB)',
                        data: [],
                        backgroundColor: 'rgba(255, 99, 132, 0.2)',
                        borderColor: 'rgba(255, 99, 132, 1)',
                        borderWidth: 1
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        xAxes: [{
                            type: 'time',
                            time: {
                                displayFormats: {
                                    second: 'DD/MM/YY HH:mm:ss' // Format de date modifié
                                }
                            },
                            distribution: 'linear',
                            ticks: {
                                source: 'auto'
                            }
                        }],
                        yAxes: [{
                            scaleLabel: {
                                display: true,
                                labelString: 'Niveau Sonore (dB)'
                            },
                            ticks: {
                                beginAtZero: true
                            }
                        }]
                    }
                }
            });
        }

        // Mettre à jour le graphique avec de nouvelles données
        function mettreAJourGraphique(timestamp, valeur) {
            graphique.data.labels.push(timestamp);
            graphique.data.datasets[0].data.push({ x: timestamp, y: valeur });
            graphique.update();

            // Afficher les données du capteur
            document.getElementById('donneesCapteur').innerHTML = '<div class="alert alert-success" role="alert">Niveau sonore actuel : ' + valeur + ' dB</div>';

            // Mettre à jour l'état des LEDs en fonction du niveau sonore
            var ledVerte = document.getElementById('ledVerte');
            var ledRouge = document.getElementById('ledRouge');

            if (valeur > 500) {
                ledVerte.classList.remove('led-allumee');
                ledRouge.classList.add('led-allumee');
            } else {
                ledRouge.classList.remove('led-allumee');
                ledVerte.classList.add('led-allumee');
            }
        }

        // Récupérer les données du serveur
        function recupererDonneesSynchrone() {
            const esp32Address = 'http://172.20.10.14'; // Utilisez l'adresse IP de votre ESP32
            var xhr = new XMLHttpRequest();
            xhr.open('GET', esp32Address + '/data.json', false);  // false pour la requête synchrone
            xhr.send();

            if (xhr.status === 200) {
                var data = JSON.parse(xhr.responseText);
                mettreAJourGraphique(new Date(), data['niveauSonore']);
            } else {
                console.error('Erreur :', xhr.status);
            }
        }

        // Appeler la fonction pour récupérer les données au chargement de la page
        window.onload = function () {
            configurerGraphique();
            recupererDonneesSynchrone(); // Récupérer les données au chargement de la page
        };
    </script>
</body>
</html>
)rawliteral";

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

  Serial.println(WiFi.localIP());
  
  // Route pour servir le contenu de index.html lors de l'accès à "/"
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", indexHtml);
  });

  // Route pour renvoyer les données du capteur sous forme de JSON
  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request) {
    int noiseLevel = analogRead(pinCapteurSonore);
    String jsonResponse = "{";
    jsonResponse += "\"niveauSonore\":" + String(noiseLevel);
    jsonResponse += "}";

    // Réponse HTTP envoyée au client
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonResponse);
    response->addHeader("Access-Control-Allow-Origin", "*"); // Ajout d'en-tête CORS
    response->addHeader("Access-Control-Allow-Methods", "GET");
    request->send(response);
  });

  server.begin();
}

void loop() {
  int niveauSonore = analogRead(pinCapteurSonore);

  if (niveauSonore > seuilSonore) {
    digitalWrite(pinLedRouge, HIGH);
    digitalWrite(pinLedVerte, LOW);
    tone(pinHautParleur, 1000); // Jouer un son
  } else {
    digitalWrite(pinLedRouge, LOW);
    digitalWrite(pinLedVerte, HIGH);
    noTone(pinHautParleur); // Arrêter le son
  }

  // Mise à jour des données toutes les 5 secondes
  delay(5000);
}

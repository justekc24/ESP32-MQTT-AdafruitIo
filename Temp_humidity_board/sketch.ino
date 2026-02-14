#include <DHT.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//Configuration wifi
#define   WLAN_SSID  "Wokwi-GUEST"
#define WLAN_PASS    ""

//Configuration AdafruitIo

#define AIO_SERVER        "io.adafruit.com"
#define AIO_SERVERPORT     1883
#define AIO_USERNAME      "justekc24io"
#define AIO_KEY           "aio_tbki71lu4I4kDDrcHOPDqgMkMMMff"


//Configuration DHT
#define DHTPIN  15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float temperature, humidity;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish temp_feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temphum.temperature");
Adafruit_MQTT_Publish hum_feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temphum.humidity");

void MQTT_connect();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  dht.begin();
  Serial.print("Conexion au WiFi...");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    delay(50);
  }
  Serial.println("CONNECTED");

}

void loop() {
  // put your main code here, to run repeatedly:
  MQTT_connect();

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)){
    Serial.println("Erreur de lecture DHT");
    return;
  }

  // Publication des données
  Serial.print("Envoi Température: ");
  Serial.println(temperature);
  if (! temp_feed.publish(temperature)) {
    Serial.println(" -> Échec");
  } else {
    Serial.println(" -> OK !");
  }

  Serial.print("Envoi Humidité: ");
  Serial.println(humidity);
  if (! hum_feed.publish(humidity)) {
    Serial.println(" -> Échec");
  } else {
    Serial.println(" -> OK !");
  }

  
  // On attend 10 secondes entre chaque envoi
  delay(500); 
}

// Fonction de gestion de la connexion MQTT
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) return;

  Serial.print("Connexion à Adafruit IO... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Nouvelle tentative dans 5 secondes...");
    mqtt.disconnect();
    delay(2000);
    retries--;
    if (retries == 0) while (1); // Stop si échec critique
  }
  Serial.println("MQTT Connecté !");
}
  


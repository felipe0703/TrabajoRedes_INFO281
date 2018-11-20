#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
#include "WifiLocation.h"
#include <FirebaseArduino.h>

#define GOOGLE_KEY "AIzaSyDP4SOZgPpJCEYormxf53cA9tnFIPoxArk" // Clave API Google Geolocation
#define WIFI_SSID "Q6_1425" // SSID de tu red WiFi
#define WIFI_PASSWORD "marcelo11" // Clave de tu red WiFi
#define FIREBASE_HOST "redesinfo281.firebaseio.com" // Host o url de Firebase
#define FIREBASE_AUTH "YC5H3n7yo3TVrt33EZDQxL0L32RJV15P6F8W5NaN"
#define LOC_PRECISION 7 // Precisi´on de latitud y longitud

// Llamada a la API de Google
WifiLocation location(GOOGLE_KEY);
location_t loc; // Estructura de datos que devuelve la librer´ıa WifiLocation

// Variables
byte mac[6];
String macStr = "";
String nombreComun = "NodeMCU";

// Cliente WiFi
WiFiClientSecure client;

void setup() {
    Serial.begin(115200);
    
    // Conexion con la red WiFi
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(WIFI_SSID);
        // Connect to WPA/WPA2 network:
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        // wait 5 seconds for connection:
        delay(5000);
        Serial.print("Status = ");
        Serial.println(WiFi.status());
    }
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    
    // Obtenemos la MAC como cadena de texto
    macStr = obtenerMac();
    Serial.print("MAC NodeMCU: ");
    Serial.println(macStr);
}

void loop() {
    // Obtenemos la geolocalizaci´on WiFi
    loc = location.getGeoFromWiFi();
    // Mostramos la informaci´on en el monitor serie
    Serial.println("Location request data");
    Serial.println(location.getSurroundingWiFiJson());
    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));
    // Hacemos la petici´on HTTP mediante el m´etodo PUT
    peticionPut();
    Firebase.setInt("LEDStatus",0);
    // Esperamos 15 segundos
    delay(15000);
}
/********** FUNCI´ON PARA OBTENER MAC COMO STRING **********/
String obtenerMac(){
    // Obtenemos la MAC del dispositivo
    WiFi.macAddress(mac);
    // Convertimos la MAC a String
    String keyMac = "";
    for (int i = 0; i < 6; i++){
        String pos = String((uint8_t)mac[i], HEX);
        if (mac[i] <= 0xF)
        pos = "0" + pos;
        pos.toUpperCase();
        keyMac += pos;    
        if (i < 5)
        keyMac += ":";
    }
    // Devolvemos la MAC en String
    return keyMac;
}

/********** FUNCION QUE REALIZA LA PETICI´ON PUT **************/
void peticionPut(){
      // Cerramos cualquier conexi´on antes de enviar una nueva petici´on
      client.stop();
      client.flush();
      // Enviamos una petici´on por SSL
      if (client.connect(FIREBASE_HOST, 443)) {
          // Petición PUT JSON
          String toSend = "PUT /dispositivo/";
          toSend += macStr;
          toSend += ".json HTTP/1.1\r\n";
          toSend += "Host:";
          toSend += FIREBASE_HOST;
          toSend += "\r\n" ;
          toSend += "Content-Type: application/json\r\n";
          String payload = "{\"fuelLevel\":";
          payload += String(64);
          payload += ",";
          payload += "\"km\":";
          payload += String(72);
          payload += ",";
          payload += "\"aceleration\":";
          payload += String(4);
          payload += ",";
          payload += "\"rpm\":";
          payload += String(3000);
          payload += ",";
          payload += "\"kmh\":";
          payload += String(50);
          payload += ",";
          payload += "\"lane\":";
          payload += String(1);
          payload += ",";
          payload += "\"nombre\": \"";
          payload += nombreComun;
          payload += "\"}";
          
          payload += "\r\n";
          toSend += "Content-Length: " + String(payload.length()) + "\r\n";
          toSend += "\r\n";
          toSend += payload;
          Serial.println(toSend); 
          client.println(toSend);
          client.println();
          client.flush();
          client.stop();
          Serial.println("Todo OK");
    } else {
        // Si no podemos conectar
        client.flush();
        client.stop();
        Serial.println("Algo ha ido mal");
    }
}

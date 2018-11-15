#include <ESP8266WiFi.h>
#include "WifiLocation.h"
#define GOOGLE_KEY "AIzaSyCPQaYqXcL6kuSyS6hxOar-MGBgtNi-DUs" // Clave API Google Geolocation
#define SSID "UACh_Alumnos" // SSID de tu red WiFi
#define PASSWD "993300390" // Clave de tu red WiFi
#define HOSTFIREBASE "redesinfo281.firebaseio.com/" // Host o url de Firebase
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
    
    // Conexi´on con la red WiFi
    while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SSID);
    
    // Connect to WPA/WPA2 network:
    WiFi.begin(SSID, PASSWD);
    // wait 5 seconds for connection:
    delay(5000);
    Serial.print("Status = ");
    Serial.println(WiFi.status());
    }
    
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
/********** FUNCI´ON QUE REALIZA LA PETICI´ON PUT **************/
void peticionPut()
{
    // Cerramos cualquier conexi´on antes de enviar una nueva petici´on
    client.stop();
    client.flush();
    // Enviamos una petici´on por SSL
    if (client.connect(HOSTFIREBASE, 443)) {
        // Petición PUT JSON
        String toSend = "PUT /dispositivo/";
        toSend += macStr;
        toSend += ".json HTTP/1.1\r\n";
        toSend += "Host:";
        toSend += HOSTFIREBASE;
        toSend += "\r\n" ;
        toSend += "Content-Type: application/json\r\n";
        String payload = "{\"lat\":";
        payload += String(loc.lat, LOC_PRECISION);
        payload += ",";
        payload += "\"lon\":";
        payload += String(loc.lon, LOC_PRECISION);
        payload += ",";
        payload += "\"prec\":";
        payload += String(loc.accuracy);
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

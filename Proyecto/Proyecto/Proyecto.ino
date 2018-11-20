// Librerías 
#include <ESP8266WiFi.h>
#include "WifiLocation.h"
#include <FirebaseArduino.h>

// Definición de variables
#define GOOGLE_KEY "AIzaSyDP4SOZgPpJCEYormxf53cA9tnFIPoxArk" // Clave API Google Geolocation
#define WIFI_SSID "Q6_1425" // SSID de tu red WiFi
#define WIFI_PASSWORD "marcelo11" // Clave de tu red WiFi
#define FIREBASE_HOST "redesinfo281.firebaseio.com" // Host o url de Firebase
#define FIREBASE_AUTH "YC5H3n7yo3TVrt33EZDQxL0L32RJV15P6F8W5NaN"
#define LOC_PRECISION 7 // Precision de latitud y longitud

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
    while ( WiFi.status() != WL_CONNECTED ) {
        //Serial.print("Attempting to connect to WPA SSID: ");
        //Serial.println(WIFI_SSID);
        
        // Connect to WPA/WPA2 network:
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        Serial.print(".");
        
        // wait 5 seconds for connection:
        delay(1000);
        
        //Serial.print("Status = ");
        //Serial.println(WiFi.status()); 
        
    }
    Serial.print("Status = ");
    Serial.println(WiFi.status());
    
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    
    // Obtenemos la MAC como cadena de texto
    macStr = obtenerMac();
    //Serial.print("MAC NodeMCU: ");
    //Serial.println(macStr);

}

void loop() {
  
   // Obtenemos la geolocalizacion WiFi
    loc = location.getGeoFromWiFi();
    
    // Mostramos la informacion en el monitor serie
//    Serial.println("Location request data");
//    Serial.println(location.getSurroundingWiFiJson());
//    Serial.println("");
//    Serial.println("Latitude: " + String(loc.lat, 7));
//    Serial.println("Longitude: " + String(loc.lon, 7));
//    Serial.println("Accuracy: " + String(loc.accuracy));
//    Serial.println("");
    
    // Hacemos la peticion HTTP mediante el metodo PUT
    peticionPut();
    
    //Firebase.setInt("LEDStatus",0);
    
    // Esperamos 15 segundos
    delay(1000);
}

/********** FUNCION PARA OBTENER MAC COMO STRING **********/
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

/********** FUNCION QUE REALIZA LA PETICION PUT **************/
void peticionPut(){
      // Cerramos cualquier conexion antes de enviar una nueva peticion
      client.stop();
      client.flush();
      
      // Enviamos una peticion por SSL
      if (client.connect(FIREBASE_HOST, 443)) {

          status_Put();
          mechanicFailure_Put();
          crash_Put();
          laneChanged_Put();
          
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
void  status_Put(){
    int fuelLevel = 11;
    int km = 25;
    int aceleration = 12;
    int rpm = 45;
    int kmh = 478;
    int lane = 1;

    // Petición PUT JSON
    String toSend = "PUT /status/";
    toSend += macStr;
    toSend += ".json HTTP/1.1\r\n";
    toSend += "Host:";
    toSend += FIREBASE_HOST;
    toSend += "\r\n" ;
    toSend += "Content-Type: application/json\r\n";
            
    String payload = "{\"fuelLevel\":";
    payload += fuelLevel;
    payload += ",";
    
    payload += "\"km\":";
    payload += km;
    payload += ",";
    
    payload += "\"aceleration\":";
    payload += aceleration;
    payload += ",";
    
    payload += "\"rpm\":";
    payload += rpm;
    payload += ",";
    
    payload += "\"kmh\":";
    payload += kmh;
    payload += ",";
    
    payload += "\"lane\":";
    payload += lane;
    payload += ",";
    
    payload += " \"nombre\":\" ";
    payload += nombreComun;
    payload += " \"} ";          
    payload += "\r\n";
    
    toSend += "Content-Length: " + String(payload.length()) + "\r\n";
    toSend += "\r\n";
    toSend += payload;

    client.println(toSend);
    client.println();
}

void  mechanicFailure_Put(){
    int failureCode = 1;

    // Petición PUT JSON
    String toSend = "PUT /mechanicFailure/";
    toSend += macStr;
    toSend += ".json HTTP/1.1\r\n";
    toSend += "Host:";
    toSend += FIREBASE_HOST;
    toSend += "\r\n" ;
    toSend += "Content-Type: application/json\r\n";
          
    String payload = "{\"failureCode\":";
    payload += failureCode;   
    payload += "}";          
    payload += "\r\n";
    toSend += "Content-Length: " + String(payload.length()) + "\r\n";
    toSend += "\r\n";
    toSend += payload;

    client.println(toSend);
    client.println();
}

void  crash_Put(){
    int aceleration = 11;
    bool airBagsActivated = true;

    // Petición PUT JSON
    String toSend = "PUT /crash/";
    toSend += macStr;
    toSend += ".json HTTP/1.1\r\n";
    toSend += "Host:";
    toSend += FIREBASE_HOST;
    toSend += "\r\n" ;
    toSend += "Content-Type: application/json\r\n";
            
    String payload = "{\"aceleration\":";
    payload += aceleration;
    payload += ",";
    
    payload += "\"airBagsActivated\":";
    payload += airBagsActivated;    
    payload += "}";          
    payload += "\r\n";
    
   toSend += "Content-Length: " + String(payload.length()) + "\r\n";
   toSend += "\r\n";
    toSend += payload;

    client.println(toSend);
    client.println();
}

void  laneChanged_Put(){
    int oldLane = 2;
    int newLane = 1;

    // Petición PUT JSON
    String toSend = "PUT /laneChanged/";
    toSend += macStr;
    toSend += ".json HTTP/1.1\r\n";
    toSend += "Host:";
    toSend += FIREBASE_HOST;
    toSend += "\r\n" ;
    toSend += "Content-Type: application/json\r\n";
            
    String payload = " {\"oldLane\": ";
    payload += oldLane;
    payload += ",";
    
    payload += " \"newLane\": ";
    payload += newLane;
    payload += " } ";          
    payload += "\r\n";
    
    toSend += "Content-Length: " + String(payload.length()) + "\r\n";
    toSend += "\r\n";
    toSend += payload;

    client.println(toSend);
    client.println();
}

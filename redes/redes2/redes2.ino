#include <FirebaseArduino.h>
#include "WifiLocation.h"

// Set these to run example.
#define FIREBASE_HOST "redesinfo281.firebaseio.com/"
#define FIREBASE_AUTH "YC5H3n7yo3TVrt33EZDQxL0L32RJV15P6F8W5NaN"
#define WIFI_SSID "WifiTelsur_CAVN" // SSID de tu red WiFi
#define WIFI_PASSWORD "27162925" // Clave de tu red WiF


void setup() { 
    Serial.begin(9600);  
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  
    Serial.print("connecting");  
    
    while (WiFi.status() != WL_CONNECTED) {  
        Serial.print(".");        
        delay(500);    
    } 

    Serial.println();    
    Serial.print("connected: ");    
    Serial.println(WiFi.localIP());    
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);    
    
}

void loop() {

    //Firebase.setInt("LEDStatus",0);
    if (Firebase.failed()){ // Check for errors {    
        Serial.print("setting /number failed:");    
        Serial.println(Firebase.error());    
        return;
    }    
    delay(1000);
}

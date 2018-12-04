#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "redesinfo281.firebaseio.com"
#define FIREBASE_AUTH "YC5H3n7yo3TVrt33EZDQxL0L32RJV15P6F8W5NaN"
//#define WIFI_SSID "Q6_1425"
//#define WIFI_PASSWORD "marcelo11"
#define WIFI_SSID "ACLECH"
#define WIFI_PASSWORD "carlosaclech910"

// Variables
int vel_inicial = 1;
int vel_final;
int aceleracion = 15;
int lane = 1;
int timer;
float dist = 0;
bool continuar = true;
unsigned long tiempoReal = 0;
unsigned long tiempo = 0;
unsigned long tiempo2 = 0;
unsigned long tiempoSegundos = 0;

void setup() {
  Serial.begin(115200);
  tiempoReal = millis();

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  //This initialize the client with the given firebase host and credentials.  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  removeAll();
}





void loop() {

  if (continuar){
    
    int probabilidad = numerosRandom(1,100);

    timer = Tiempo();
    
    vel_final = velocidad(aceleracion,vel_inicial,timer);
    
//    if (vel_final >= 150){
//      tiempo2 = tiempoReal; 
//    }
    dist = distancia(vel_final, tiempoSegundos);
  
    if( probabilidad <= 20){
       MechanicFailure_Put();  
    }
    if( probabilidad <= 50){
      Crash_Put(aceleracion);
    }
    if( probabilidad <= 80){
      lane = LaneChanged_Put(lane);
    } 
    Status_Put(lane,aceleracion,vel_final,dist);
    delay(1000); 
    
  }
  //Status_Get(); 
  
  
  
//  // remove value
//  Firebase.remove("number");
//  delay(1000);
 
// append a new value to /logs
//  String name = Firebase.pushInt("logs", n++);
//  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("pushing /logs failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }


}

int Tiempo(){
    tiempoReal = millis(); //Actualiza el tiempo actual    
    tiempo = tiempoReal - tiempo2; 
    tiempoSegundos = tiempo/1000;

    return tiempoSegundos;
}

void removeAll(){
  Firebase.remove("status");
  Firebase.remove("mechanicFailure");
  Firebase.remove("crash");
  Firebase.remove("laneChanged");
}

int numerosRandom(int minimo, int maximo){
  int number;
  number = random(minimo,maximo + 1);
  return number;
}

int velocidad(int acel,int vel_ini,int tiempo){
  int vel_final = acel + (vel_ini * tiempo);
  if(vel_final >= 150){
    vel_final = 150;  
  }
  return vel_final;
}
float distancia( int velocidad, int tiempo){
    float dist = float((velocidad * tiempo)/60);
    return dist;
}

void Status_Get(){

  int fuelLevel = Firebase.getInt("status/fuelLevel");
  float km = Firebase.getInt("status/km");
  int aceleration = Firebase.getInt("status/aceleration");
  int rpm = Firebase.getInt("status/rpm");
  int kmh = Firebase.getInt("status/kmh");
  int lane = Firebase.getInt("status/lane");

  Serial.println("**************************************************************");
  Serial.print("fuelLevel: ");
  Serial.println(fuelLevel);
  Serial.print("km: ");
  Serial.println(km);
  Serial.print("aceleration: ");
  Serial.println(aceleration);
  Serial.print("rpm: ");
  Serial.println(rpm);
  Serial.print("lane: ");
  Serial.println(lane);
}

void Status_Put(int lane, int aceleration, int velocidad, float distancia){

  int fuelLevel = numerosRandom(0,99);
  //int km = numerosRandom(0,149);
  //int aceleration = numerosRandom(0,99);
  int rpm = numerosRandom(1,500);
  //int kmh = numerosRandom(1,100);
  //int lane = numerosRandom(1,2);

  // set value fuellLevel
  Firebase.setInt("status/fuelLevel", fuelLevel);
  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /fuelLevel failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }

  // set value km
  Firebase.setFloat("status/km", distancia);
  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /km failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }

  // set value aceleration
  Firebase.setInt("status/aceleration", aceleration);
  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /aceleration failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }

  // set value rpm
  Firebase.setInt("status/rpm", rpm);
  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /rpm failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }

  // set value kmh
  Firebase.setInt("status/kmh", velocidad);
  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /kmh failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }

  // set value lane
  Firebase.setInt("status/lane", lane);
  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /lne failed:");
//      Serial.println(Firebase.error());  
//      return;
//  } 

}


void MechanicFailure_Put(){

  int failure = numerosRandom(1,2);
    
  // set value failureCode
  Firebase.setInt("mechanicFailure/failureCode", failure);
  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /failureCode failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
}

//**********************************************//
void Crash_Put(int aceleracion){

  int air = numerosRandom(1,3);
  bool air_act;
  
  if(air == 1){
    air_act = true;
  }else{
    air_act = false;
  }
  // set value aceleration
  Firebase.setInt("crash/aceleration", aceleracion);
  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /aceleration failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }

  // set value airBagsActivated
  Firebase.setBool("crash/airBagsActivated", air_act);
  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /airBagsActivated failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
  //continuar = false;
}

//**********************************************//
int LaneChanged_Put(int old_lane){

    int new_lane = numerosRandom(1,2); 

    // cambio de carril
    if(old_lane != new_lane){
         // set value oldLane
        Firebase.setInt("laneChanged/oldLane", old_lane);
        
//        // handle error
//        if (Firebase.failed()) {
//            Serial.print("setting /oldLane failed:");
//            Serial.println(Firebase.error());  
//            //return;
//        }
    }  
    // set value newLane
    Firebase.setInt("laneChanged/newLane", new_lane);
    
//    // handle error
//    if (Firebase.failed()) {
//        Serial.print("setting /newLane failed:");
//        Serial.println(Firebase.error());  
//        //return;
//    }
    return new_lane;
}

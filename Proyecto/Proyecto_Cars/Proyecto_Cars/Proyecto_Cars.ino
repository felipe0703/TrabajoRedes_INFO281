// librerias
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>

// Credenciales
#define FIREBASE_HOST "redesinfo281.firebaseio.com"
#define FIREBASE_AUTH "YC5H3n7yo3TVrt33EZDQxL0L32RJV15P6F8W5NaN"
//#define WIFI_SSID "Marcelo_Sebastian"
//#define WIFI_PASSWORD "marsexo11"
#define WIFI_SSID "iPhone de Felipe"
#define WIFI_PASSWORD "martinaa"
//#define WIFI_SSID "WIFITELSUR_ANAS"
//#define WIFI_PASSWORD "76020949"

//Objetos 
DynamicJsonBuffer jBuffer;
JsonObject& json_data = jBuffer.createObject(); // data
JsonObject& json_status = jBuffer.createObject(); // status
JsonObject& json_mechanicF = jBuffer.createObject(); // mechanicFailure
JsonObject& json_crash = jBuffer.createObject(); // crash
JsonObject& json_lane = jBuffer.createObject(); // laneChange
JsonArray& data_set = json_data.createNestedArray("data");



// Valores iniciales consideradas como hipotesis
float speedI = 0; // Unidad de medida: [km/h]. Velocidad inicial
float speedF = 10; // Unidad de medida: [km/h]. Velocidad final
float timer = 0.000278; // conversion de 1 segundo a horas
float aceleration;
float distanceI = 0.00278; // Unidad de medida: [km]. Distancia inicial recorrida
float distanceF; // Unidad de medida: [km]. Distancia final recorrida
float distance = 0; // Unidad de medida: [km]. Distancia acumulada
float fuel = 58; // Unidad de medida: [litros]. Capacidad del tanque
int comportC = 1; // comportamiento aleatorio del coche
float rpm;
float lane = 1;



void setup() {
    Serial.begin(115200);
  
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
}

void loop() {
  // generar probabilidad de ocurrencia de los eventos
  int probabilidad1 = random(1,100);
  int probabilidad2 = random(1,100);
  int probabilidad3 = random(1,100);
  
  if( probabilidad1 <= 20){
    //Serial.print("mechanicFailure: ");
    //Serial.println(probabilidad1);
    MechanicFailure_Put();  
  }
  if( probabilidad2 <= 50){
//    Serial.print("crash: ");
//    Serial.println(probabilidad2);
    //Crash_Put();
  }
  if( probabilidad3 <= 80){
//    Serial.print("laneChanged: ");
//    Serial.println(probabilidad3);
    //lane = LaneChanged_Put(lane);
  }
  delay(1000);
  //Status_Put(lane);
}


//**************************************************************************************************************
//***                             Métodos que generan datos ficticios                                        ***
//**************************************************************************************************************

//**********************************************************
//***************   Generador de Velocidad   ***************
//**********************************************************

// La idea es que vaya variando la velocidad. 
// Cuando llega a los 100[km/h] este desciende hasta los 50[km/h] y vuelva a ascender

float generateSpeed(){ 
   
  float contRandP = random(1,5); // aumenta la velocidad de forma lenta
  float contRandS = random(5,8); // disminuye la velocidad de forma drastica
  
  if(comportC == 1) {
      if(speedF <= 100){        
        speedF = speedF + contRandP; // velocidad ascendente
      }
      else{        
        comportC = 2;
      }
      //Serial.println(speedI); //Imprime velocidad inicial en el monitor serial
      //Serial.println(speedF); //Imprime velocidad finalen el monitor serial      
  }
  else{
      if(speedF >= 50){        
        speedF = speedF - contRandS; // velocidad ascendente      
      }
      else{        
        comportC = 1;
      }
      //Serial.println(speedI); //Imprime velocidad inicial en el monitor serial
      //Serial.println(speedF); //Imprime velocidad finalen el monitor serial     
  }
  return speedF;  
}

//**********************************************************
//**************   Generador de Aceleración   **************
//**********************************************************

float generateAcel(float speedI, float speedF){ 
   
  aceleration = (speedF-speedI)/timer; // Los resultado son numeros grandes. si haces la conversion a [m/s^2] tendran mayor sentido
  return aceleration;
}

//**********************************************************
//***************   Generador de Distancia   ***************
//**********************************************************

float generateDistance(float speedI, float distanceI, float aceleration){
  
  distanceF = distanceI + speedI*timer+ 0.5*aceleration*(pow(timer,2)); // xf = xi + vi*t + (1/2)*a*(t^2)
  return distanceF;
}

//**********************************************************
//**************   Generador de Combustible   **************
//**********************************************************

// Tanque del Mazda CX-5: 58 [l] (Litros)
// Consumo: 5.8[l] cada 100[km] => 1 litro 
float generateFuelLevel(float distanceX){
  
  float fuelLevelStatus;   
  fuelLevelStatus = distanceX/17.24; // se obtiene mediante regla de 3. 1 litro ----> 17.24 km; x litros ----> distance
  fuel = fuel - fuelLevelStatus;
  return fuel;
}

//**********************************************************
//******************   Generador de RPM   ******************
//**********************************************************

// Dimensiones del neumatico: 225/55 R19, donde:
// 225 es la anchura del neumatico en [mm]
// 55 es el porcentaje de la anchura
// 19 el diametro de la rueda en [pulg]
// Datos obtenido en la ficha del Mazda CX-5
// 1ª: 3.552
// 2ª: 2.022
// 3ª: 1.452
// 4ª: 1.00
// 5ª: 0.708
// 6ª: 0.599
// Relacion final: 4.090
float generateRPM(float speedFX){
  
  float dataN = ((225/1000)*(55/100) + (19*0.0127))*6.2832;
  //Serial.println(dataN); //Imprime velocidad finalen el monitor serial 
  float RF1 = 4.09*3.552;
  float RF2 = 4.09*2.022;
  float RF3 = 4.09*1.452;
  float RF4 = 4.09*1.00;
  float RF5 = 4.09*0.708;
  float RF6 = 4.09*0.599;
  float relationSpeed;
  
  if(speedFX <=15){ //1era
    relationSpeed = (dataN*60)/RF1;
    //Serial.println(relationSpeed); //Imprime velocidad finalen el monitor serial 
    rpm = (speedFX/relationSpeed)*1000;    
  }
  else if(speedFX >=16 && speedFX <= 30){ // 2da
    relationSpeed = (dataN*60)/RF2;
    rpm = (speedFX/relationSpeed)*1000;
  }
  else if(speedFX >=31 && speedFX <= 43){ //3era
    relationSpeed = (dataN*60)/RF3;
    rpm = (speedFX/relationSpeed)*1000;
  }
  else if(speedFX >=44 && speedFX <= 62){ //4ta
    relationSpeed = (dataN*60)/RF4;
    rpm = (speedFX/relationSpeed)*1000;
  }
  else if(speedFX >=63 && speedFX <=88){ // 5ta
    relationSpeed = (dataN*60)/RF5;
    rpm = (speedFX/relationSpeed)*1000;
  }
  else if(speedFX >=89 && speedFX <=200){ // 6ta
    relationSpeed = (dataN*60)/RF6;
    rpm = (speedFX/relationSpeed)*1000;
  }
  
  return rpm;
}

//**************************************************************************************************************
//***                               Métodos que generan los eventos                                          ***
//**************************************************************************************************************


//**********************************************************
//********************   Método Status   *******************
//**********************************************************

void Status_Put(int lane){ 
  
  aceleration = generateAcel(speedI, speedF);  
  distanceF = generateDistance(speedI, distanceI, aceleration);
  distance = distance + distanceF; // Distancia acumulada/recorrida
  fuel = generateFuelLevel(distanceF);
  rpm = generateRPM(speedF);

  json_data["carId"] = "MAZDA CX-5";
  json_data["timestamp"]="";
  json_data["eventType"] = "status";
  
  
  json_status["fuelLevel"] = fuel;
  json_status["km"] = distance;
  json_status["aceleration"] = aceleration;
  json_status["rpm"] = rpm;
  json_status["kmh"] = speedF;
  json_status["lane"] = lane;

//  data_set.remove(0);
//  data_set.add(json_status); 

  json_data["data"] = json_status;
  json_data.prettyPrintTo(Serial);
  Serial.println("");

// set value status
  Firebase.push("",json_data);
    
  speedI = speedF; // se iguala la velocidad inicial y velocidad final 
  distanceI = distanceF; // se iguala la distancia inicial y la distanciafinal 
  speedF = generateSpeed();// se modifica la velocidad final despues de 1 segundo  

}


//**********************************************************
//**************   Método MechanicFailure   ****************
//**********************************************************

void MechanicFailure_Put(){

  int failure = random(1,2);

  json_data["carId"] = "MAZDA CX-5";
  json_data["timestamp"]="";
  json_data["eventType"] = "mechanicFailure";
    
  json_mechanicF["failureCode"] = failure;

//  data_set.remove(0);
//  data_set.add(json_mechanicF);  
  json_data["data"] = json_mechanicF;
  json_data.prettyPrintTo(Serial);
  Serial.println("");
  
  // set value failureCode
  Firebase.push("",json_data);
   
}


//**********************************************************
//*******************   Método Crash   *********************
//**********************************************************

void Crash_Put(){

  int air = random(1,2);
  bool air_act;
  float desaceleration = random(2500, 6000);
  
  desaceleration = desaceleration - aceleration;  //desaceleracion beta
  
  if(air == 1){
    air_act = true;
  }else{
    air_act = false;
  }

  json_data["carId"] = "MAZDA CX-5";
  json_data["timestamp"]="";
  json_data["eventType"] = "crash";
  
  json_crash["aceleration"] = desaceleration;
  json_crash["airbagsActivated"] = air_act;
  
//  data_set.remove(0);
//  data_set.add(json_crash); 
  json_data["data"] = json_crash; 
  json_data.prettyPrintTo(Serial);
  Serial.println("");
  
  // set value failureCode
  Firebase.push("",json_data);
}


//**********************************************************
//****************   Método LaneChanged   ******************
//**********************************************************


int LaneChanged_Put(int old_lane){

    int new_lane = random(1,2); 

    json_data["carId"] = "MAZDA CX-5";
    json_data["timestamp"]="";
    json_data["eventType"] = "laneChanged";

    // cambio de carril
    if(old_lane != new_lane){
      
         // set value oldLane
        json_lane["oldLane"] = old_lane;
    } 
    // set value newLane  

    
    json_lane["newLane"] = new_lane;
    
//    data_set.remove(0);
//    data_set.add(json_lane);  

    json_data["data"] = json_lane; 
    json_data.prettyPrintTo(Serial);
    Serial.println("");
    
    // set value failureCode
    Firebase.push("",json_data);
    
    return new_lane;
}

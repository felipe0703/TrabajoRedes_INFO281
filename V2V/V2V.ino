#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>

// Set these to run example.
#define HOSTFIREBASE "redeswifi-4edab.firebaseio.com" //
#define SSID "Marcelo_Sebastian" // SSID de tu red WiFi
#define PASSWD "marsexo11" // Clave de tu red WiFi

//Objetos 
DynamicJsonBuffer jBuffer;
JsonObject& json = jBuffer.createObject(); // data
JsonObject& json1 = jBuffer.createObject(); // laneChange
JsonObject& json2 = jBuffer.createObject(); // status
JsonObject& json3 = jBuffer.createObject(); // mechanicFailure
JsonObject& json4 = jBuffer.createObject(); // crash
JsonObject& json5 = jBuffer.createObject(); // laneChange

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
  WiFi.begin(SSID, PASSWD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(HOSTFIREBASE);
}

void loop() {
  Status();
  int probabilidad1 = random(1,100);
  int probabilidad2 = random(1,100);
  int probabilidad3 = random(1,100);

  if( probabilidad1 <= 20){
    Serial.print("mechanicFailure: ");
    Serial.println(probabilidad1);
     mechanicFailure();  
  }
  if( probabilidad2 <= 50){
    Serial.print("crash: ");
    Serial.println(probabilidad2);
    crash();
  }
  if( probabilidad3 <= 80){
    Serial.print("laneChanged: ");
    Serial.println(probabilidad3);
    lane = laneChanged();
  }
}
  
void Status(){ 
  
  aceleration = generateAcel(speedI, speedF);  
  distanceF = generateDistance(speedI, distanceI, aceleration);
  distance = distance + distanceF; // Distancia acumulada/recorrida
  fuel = generateFuelLevel(distanceF);
  rpm = generateRPM(speedF);

  //json["cardId"] = json1;
  json1["carId"] = "MAZDA CX-5";
  json["data"] = json2;
  
  json["timestamp"]="";
  json["EventType"] = "status";
  json2["fuelLevel"] = fuel;
  json2["km"] = distance;
  json2["aceleration"] = aceleration;
  json2["rpm"] = rpm;
  json2["kmh"] = speedF;
  json2["lane"] = lane;
  
  Firebase.push("",json);
  
  delay(1000); // espera un segundo  
  
  speedI = speedF; // se iguala la velocidad inicial y velocidad final 
  distanceI = distanceF; // se iguala la distancia inicial y la distanciafinal 
  speedF = generateSpeed();// se modifica la velocidad final despues de 1 segundo
   

}

 // la idea es que vaya variando la velocidad. Cuando llega a los 100[km/h] este desciende hasta los 50[km/h] y vuelva a ascender
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
      Serial.println(speedI); //Imprime velocidad inicial en el monitor serial
      Serial.println(speedF); //Imprime velocidad finalen el monitor serial      
  }
  else{
      if(speedF >= 50){        
        speedF = speedF - contRandS; // velocidad ascendente      
      }
      else{        
        comportC = 1;
      }
      Serial.println(speedI); //Imprime velocidad inicial en el monitor serial
      Serial.println(speedF); //Imprime velocidad finalen el monitor serial     
  }
  return speedF;  
}

float generateAcel(float speedI, float speedF){ 
   
  aceleration = (speedF-speedI)/timer; // Los resultado son numeros grandes. si haces la conversion a [m/s^2] tendran mayor sentido
  return aceleration;
}

float generateDistance(float speedI, float distanceI, float aceleration){
  
  distanceF = distanceI + speedI*timer+ 0.5*aceleration*(pow(timer,2)); // xf = xi + vi*t + (1/2)*a*(t^2)
  return distanceF;
}

// Tanque del Mazda CX-5: 58 [l] (Litros)
// Consumo: 5.8[l] cada 100[km] => 1 litro 
float generateFuelLevel(float distanceX){
  
  float fuelLevelStatus;   
  fuelLevelStatus = distanceX/17.24; // se obtiene mediante regla de 3. 1 litro ----> 17.24 km; x litros ----> distance
  fuel = fuel - fuelLevelStatus;
  return fuel;
}

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
  Serial.println(dataN); //Imprime velocidad finalen el monitor serial 
  float RF1 = 4.09*3.552;
  float RF2 = 4.09*2.022;
  float RF3 = 4.09*1.452;
  float RF4 = 4.09*1.00;
  float RF5 = 4.09*0.708;
  float RF6 = 4.09*0.599;
  float relationSpeed;
  
  if(speedFX <=15){ //1era
    relationSpeed = (dataN*60)/RF1;
    Serial.println(relationSpeed); //Imprime velocidad finalen el monitor serial 
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

void mechanicFailure(){
  
 int failure = random(1,2);
 json["data"] =json3;
 json["EventType"] = "mechanicFailure";
 json3["failureCode"] = failure; 
 Firebase.push("",json);
 
}

//**********************************************//
void crash(){
  
  int air = random(1,2);
  bool air_act;
  float desaceleration = random(2500, 6000);
  
  // get value 
  Serial.print("Aceleracion : ");
  Serial.println(Firebase.getFloat("aceleration")); 

  desaceleration = desaceleration - aceleration;  //desaceleracion beta
  
  if(air == 1){
    air_act = true;
  }else{
    air_act = false;
  }
  
  json["Data" ]= json4;
  json["EventType"] = "crash";
  json4["aceleration"] = desaceleration;
  json4["airbagsActivated"] = air_act;
 
  Firebase.push("",json);
}

//**********************************************//
float laneChanged(){

  int old_l = lane;
  int new_l = random(1,2);

  json["Data"] = json5;
  json["EventType"] = "LaneChange";
  json5["oldLane"] = old_l;
  json5["newLane"] = new_l;
  
  Firebase.push("",json);
}

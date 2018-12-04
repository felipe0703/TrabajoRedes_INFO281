#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>

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
unsigned long epoch; // variable global que indica el tiempo en segundos

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
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  Serial.println("Connected Firebase");
  Firebase.begin(HOSTFIREBASE);
}

//Your UTC Time Zone Differance  India -3:00
char HH = -3;
char MM = 00;


unsigned int localPort = 2390;      // local port to listen for UDP packets

/* Don't hardwire the IP address or we won't get the benefits of the pool.
 *  Lookup the IP address for the host name instead */
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server

IPAddress timeServerIP; // time.nist.gov NTP server address

const char* ntpServerName = "ntp.shoa.cl";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;


unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");  
  // establece todos los bytes en el búfer a 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);  
  
  // Inicializa los valores necesarios para formar la solicitud NTP
  
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
    
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

// extrae el horario en segundos.
// se llama esta funcion en la funcion loop.
// existe una variable global que se menciona en la parte de arriba del programa "unsigned long epoch"
void timeStamp(){
  
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP); // enviar un paquete NTP a un servidor de tiempo  
  // esperar para ver si hay una respuesta disponible
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);    
    // Se recibe el paquete y lee los datos de él
    udp.read(packetBuffer, NTP_PACKET_SIZE); // leer el paquete en el búffer
    
    // timestamp comienza en el byte 40 del paquete recibido y es de cuatro bytes,
    // o dos palabras, largas. Primero, extrae las dos palabras:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    
    // combina los cuatro bytes (dos palabras) en un entero largo
    // este es el tiempo NTP (segundos desde el 1 de enero de 1900):
    
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // ahora convierte el tiempo NTP en tiempo diario:
    Serial.print("Unix time = ");
    // Unix time comienza on Jan 1 1970. En segundos, que es 2208988800:
    const unsigned long seventyYears = 2208988800UL;    
    // restar setenta años:
    epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);
  }    
}

void loop() {
  timeStamp();
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
  
  json["timestamp"]=epoch;
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
  Serial.println(dataN); //Imprime velocidad final en el monitor serial 
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

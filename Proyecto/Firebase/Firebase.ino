//
// Copyright 2015 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// FirebaseDemo_ESP8266 is a sample that demo the different functions
// of the FirebaseArduino API.

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "redesinfo281.firebaseio.com"
#define FIREBASE_AUTH "YC5H3n7yo3TVrt33EZDQxL0L32RJV15P6F8W5NaN"
#define WIFI_SSID "Q6_1425"
#define WIFI_PASSWORD "marcelo11"

void setup() {
  Serial.begin(9600);

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
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;

void loop() {
  
  int probabilidad = numerosRandom(1,100);
  Status();

  if( probabilidad <= 20){
    Serial.print("mechanicFailure: ");
    Serial.println(probabilidad);
     MechanicFailure();  
  }
  if( probabilidad <= 50){
    Serial.print("crash: ");
    Serial.println(probabilidad);
    Crash();
  }
  if( probabilidad <= 80){
    Serial.print("laneChanged: ");
    Serial.println(probabilidad);
    LaneChanged();
  } 
  
  delay(1000);
  
  

//  // get value 
//  Serial.print("number get: ");
//  Serial.println(Firebase.getFloat("number"));
//  delay(1000);
//
//  // remove value
//  Firebase.remove("number");
//  delay(1000);
//
//  // set string value
//  Firebase.setString("message", "hello world");
//  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /message failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
//  delay(1000);
//  
//  
//
//  // append a new value to /logs
//  String name = Firebase.pushInt("logs", n++);
//  
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("pushing /logs failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
//  Serial.print("pushed: /logs/");
//  Serial.println(name);
//  delay(1000);
}

int numerosRandom(int minimo, int maximo){
  int number;
  number = random(minimo,maximo + 1);
  return number;
}

  
void Status(){

  int fuelLevel = numerosRandom(0,99);
  int km = numerosRandom(0,149);
  int aceleration = numerosRandom(0,99);
  int rpm = numerosRandom(1,500);
  int kmh = numerosRandom(1,100);
  int lane = numerosRandom(1,2);

  // set value fuellLevel
  Firebase.setInt("fuelLevel", fuelLevel);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /fuelLevel failed:");
      Serial.println(Firebase.error());  
      return;
  }

  // set value km
  Firebase.setInt("km", km);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /km failed:");
      Serial.println(Firebase.error());  
      return;
  }

  // set value aceleration
  Firebase.setInt("aceleration", aceleration);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /aceleration failed:");
      Serial.println(Firebase.error());  
      return;
  }

  // set value rpm
  Firebase.setInt("rpm", rpm);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /rpm failed:");
      Serial.println(Firebase.error());  
      return;
  }

  // set value kmh
  Firebase.setInt("kmh", kmh);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /kmh failed:");
      Serial.println(Firebase.error());  
      return;
  }

  // set value lane
  Firebase.setInt("lane", lane);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /lne failed:");
      Serial.println(Firebase.error());  
      return;
  } 

}


void MechanicFailure(){

  int failure = numerosRandom(1,3);
  //Serial.println(prob_ocurrencia);
  
  // set value failureCode
  Firebase.setInt("failureCode", failure);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /failureCode failed:");
      Serial.println(Firebase.error());  
      return;
  }
}

//**********************************************//
void Crash(){

  int acel = numerosRandom(1,3);
  int air = numerosRandom(1,3);
  bool air_act;
  
  if(air == 1){
    air_act = true;
  }else{
    air_act = false;
  }
  // set value aceleration
  Firebase.setInt("aceleration_crash", acel);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /aceleration failed:");
      Serial.println(Firebase.error());  
      return;
  }

  // set value airBagsActivated
  Firebase.setBool("airBagsActivated", air_act);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /airBagsActivated failed:");
      Serial.println(Firebase.error());  
      return;
  }
}

//**********************************************//
void LaneChanged(){

  int old_l = numerosRandom(1,2);
  int new_l = numerosRandom(1,2);
  
  
  // set value oldLane
  Firebase.setInt("oldLane", old_l);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /oldLane failed:");
      Serial.println(Firebase.error());  
      return;
  }

  // set value newLane
  Firebase.setInt("newLane", new_l);
  
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /newLane failed:");
      Serial.println(Firebase.error());  
      return;
  }
}

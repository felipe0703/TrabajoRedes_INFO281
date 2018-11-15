void setup() {
  // put your setup code here, to run once:
  // Inicializamos el pin 13 como Salida
  pinMode(13, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.begin(115200);
  digitalWrite(13, HIGH); // Prendemos el led (HIGH es el nivel de voltaje)
  delay(1000); // Esperamos 1 segundo
  digitalWrite(13, LOW); // apagamos el led bajando el voltaje
  delay(1000); // Esperamos otro segundo
}

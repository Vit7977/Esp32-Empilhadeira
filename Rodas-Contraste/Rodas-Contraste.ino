#include <WiFi.h>

#define motorD1 5
#define motorD2 6

#define motorE1 7
#define motorE2 8

#define velMotorE 1
#define velMotorD 2

#define sensorContraste 4

int vel = 100;

void setup() {
  Serial.begin(115200);

  pinMode(motorD1, OUTPUT);
  pinMode(motorD2, OUTPUT);
  pinMode(motorE1, OUTPUT);
  pinMode(motorE2, OUTPUT);

  pinMode(velMotorE, OUTPUT);
  pinMode(velMotorD, OUTPUT);

  pinMode(sensorContraste, INPUT);
}

void loop() {

  int sensor = digitalRead(sensorContraste);

  int potencia = map(vel, 0, 100, 0, 255);

  analogWrite(velMotorD, potencia);
  analogWrite(velMotorE, potencia);

  // Sensor detectou contraste
  if (sensor == HIGH) {

    // Roda direita gira para frente
    digitalWrite(motorD1, HIGH);
    digitalWrite(motorD2, LOW);

    // Roda esquerda parada
    digitalWrite(motorE1, LOW);
    digitalWrite(motorE2, LOW);

    Serial.println("Contraste detectado - roda direita ligada");

  } else {

    // Roda direita parada
    digitalWrite(motorD1, LOW);
    digitalWrite(motorD2, LOW);

    // Roda esquerda parada
    digitalWrite(motorE1, LOW);
    digitalWrite(motorE2, LOW);

    Serial.println("Sem contraste - motores parados");
  }

  delay(50);
}

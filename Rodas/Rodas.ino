#include <WiFi.h>

#define motorD1 5
#define motorD2 6

#define motorE1 7
#define motorE2 8

#define velMotorE 1
#define velMotorD 2

int vel=0;

const char* ssid =  "Lab_Esp32";
const char* password = "";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");

  // Aguarda a conexão ser estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Conectado com sucesso
  Serial.println();
  Serial.println("Conectado com sucesso!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  pinMode(motorD1, OUTPUT);
  pinMode(motorD2, OUTPUT);
  pinMode(motorE1, OUTPUT);
  pinMode(motorE2, OUTPUT);
  pinMode(velMotorE, OUTPUT);
}

void loop() {
  vel = 50;
  int potencia = map(vel, 0, 100, 0, 255);
  analogWrite(velMotorD, potencia);
  analogWrite(velMotorE, potencia);

  digitalWrite(motorE1, HIGH);
  digitalWrite(motorE2, LOW);
  delay(1000);
  digitalWrite(motorE1, LOW);
  digitalWrite(motorE2, HIGH);
  delay(1000);
  digitalWrite(motorE1, LOW);
  digitalWrite(motorE2, LOW);
  delay(1000);

}

#include <WiFi.h>

#define trig 1
#define echo 2

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

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
    // Envia um pulso de 10 microssegundos para o Trig
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // Mede o tempo de resposta no pino Echo
  long duracao = pulseIn(echo, HIGH);
  
  // Calcula a distância em cm (velocidade do som = 0.0343 cm/us)
  float distancia = duracao * 0.0343 / 2;

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");
  
  delay(500);
}

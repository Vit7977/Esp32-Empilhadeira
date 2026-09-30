// =====================================================
// ESP32-C3 Super Mini + L298N
// Controle de 2 motores
// =====================================================

// -------- MOTOR DIREITO --------
#define motorD1 8
#define motorD2 9
#define velMotorD 10       // ENA

// -------- MOTOR ESQUERDO --------
#define motorE1 6
#define motorE2 7
#define velMotorE 5        // ENB

// Velocidade: 0 a 100%
int velocidade = 80;


// =====================================================
// CONFIGURAÇÃO
// =====================================================
void setup() {

  Serial.begin(115200);

  // Pinos do motor direito
  pinMode(motorD1, OUTPUT);
  pinMode(motorD2, OUTPUT);

  // Pinos do motor esquerdo
  pinMode(motorE1, OUTPUT);
  pinMode(motorE2, OUTPUT);

  // Pinos de velocidade
  pinMode(velMotorD, OUTPUT);
  pinMode(velMotorE, OUTPUT);

  // Começa parado
  parar();

  Serial.println("ESP32-C3 + L298N");
  Serial.println("Sistema iniciado!");
}


// =====================================================
// LOOP
// =====================================================
void loop() {

  // -----------------------------
  // FRENTE
  // -----------------------------
  frente();
  delay(2000);

  // -----------------------------
  // PARAR
  // -----------------------------
  parar();
  delay(1000);

  // -----------------------------
  // TRÁS
  // -----------------------------
  tras();
  delay(2000);

  // -----------------------------
  // PARAR
  // -----------------------------
  parar();
  delay(1000);

  // -----------------------------
  // ESQUERDA
  // -----------------------------
  esquerda();
  delay(1500);

  // -----------------------------
  // PARAR
  // -----------------------------
  parar();
  delay(1000);

  // -----------------------------
  // DIREITA
  // -----------------------------
  direita();
  delay(1500);

  // -----------------------------
  // PARAR
  // -----------------------------
  parar();
  delay(2000);
}


// =====================================================
// DEFINIR VELOCIDADE
// =====================================================
void definirVelocidade(int v) {

  // Garante que fique entre 0 e 100
  v = constrain(v, 0, 100);

  int potencia = map(v, 0, 100, 0, 255);

  analogWrite(velMotorD, potencia);
  analogWrite(velMotorE, potencia);
}


// =====================================================
// FRENTE
// =====================================================
void frente() {

  definirVelocidade(velocidade);

  // Motor direito
  digitalWrite(motorD1, HIGH);
  digitalWrite(motorD2, LOW);

  // Motor esquerdo
  digitalWrite(motorE1, HIGH);
  digitalWrite(motorE2, LOW);

  Serial.println("FRENTE");
}


// =====================================================
// TRÁS
// =====================================================
void tras() {

  definirVelocidade(velocidade);

  // Motor direito
  digitalWrite(motorD1, LOW);
  digitalWrite(motorD2, HIGH);

  // Motor esquerdo
  digitalWrite(motorE1, LOW);
  digitalWrite(motorE2, HIGH);

  Serial.println("TRAS");
}


// =====================================================
// ESQUERDA
// =====================================================
void esquerda() {

  definirVelocidade(velocidade);

  // Motor direito gira para frente
  digitalWrite(motorD1, HIGH);
  digitalWrite(motorD2, LOW);

  // Motor esquerdo gira para trás
  digitalWrite(motorE1, LOW);
  digitalWrite(motorE2, HIGH);

  Serial.println("ESQUERDA");
}


// =====================================================
// DIREITA
// =====================================================
void direita() {

  definirVelocidade(velocidade);

  // Motor direito gira para trás
  digitalWrite(motorD1, LOW);
  digitalWrite(motorD2, HIGH);

  // Motor esquerdo gira para frente
  digitalWrite(motorE1, HIGH);
  digitalWrite(motorE2, LOW);

  Serial.println("DIREITA");
}


// =====================================================
// PARAR
// =====================================================
void parar() {

  digitalWrite(motorD1, LOW);
  digitalWrite(motorD2, LOW);

  digitalWrite(motorE1, LOW);
  digitalWrite(motorE2, LOW);

  analogWrite(velMotorD, 0);
  analogWrite(velMotorE, 0);

  Serial.println("PARADO");
}
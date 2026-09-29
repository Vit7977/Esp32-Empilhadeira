// ===============================
// PINOS
// ===============================

// HC-SR04
#define TRIG 5
#define ECHO 6

// TCRT5000
#define TCRT 2

// L298N
#define ENA 10
#define IN1 8
#define IN2 9

// ===============================
// CONFIGURAÇÕES
// ===============================

#define VELOCIDADE 100
#define DISTANCIA_OBSTACULO 20


void setup() {

  // HC-SR04
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // TCRT5000
  pinMode(TCRT, INPUT);

  // L298N
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pararMotor();

  Serial.begin(9600);
}


void loop() {

  // Lê o TCRT5000
  int contraste = digitalRead(TCRT);

  // Mede distância
  float distancia = medirDistancia();


  // =====================================
  // 1 - PRIMEIRA PRIORIDADE: OBSTÁCULO
  // =====================================

  if (distancia <= DISTANCIA_OBSTACULO) {

    pararMotor();

    Serial.println("OBSTACULO DETECTADO - PARADO");
  }


  // =====================================
  // 2 - TCRT5000 DETECTANDO CONTRASTE
  // =====================================

  else if (contraste == LOW) {

    andar();

    Serial.println("CONTRASTE DETECTADO - ANDANDO");
  }


  // =====================================
  // 3 - SEM CONTRASTE
  // =====================================

  else {

    pararMotor();

    Serial.println("SEM CONTRASTE - PARADO");
  }


  delay(20);
}


// =====================================
// MEDIR DISTÂNCIA
// =====================================

float medirDistancia() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long tempo = pulseIn(ECHO, HIGH, 30000);

  // Se não recebeu resposta
  if (tempo == 0) {
    return 999;
  }

  float distancia = tempo * 0.0343 / 2;

  return distancia;
}


// =====================================
// ANDAR PARA FRENTE
// =====================================

void andar() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  analogWrite(ENA, VELOCIDADE);
}


// =====================================
// PARAR MOTOR
// =====================================

void pararMotor() {

  analogWrite(ENA, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

/*
  Empilhadeira - controle por Bluetooth (BLE) pelo app (ESP32 + L298N)

  Como funciona:
  - O ESP anuncia o Bluetooth com o nome "Empilhadeira".
  - O app (aba Joystick) procura esse nome, conecta e envia "x,y,elevacao" ~10x por segundo.
    x: -100 (esquerda) a 100 (direita)   y: -100 (ré) a 100 (frente)
  - Se os comandos pararem de chegar (Bluetooth caiu, app fechou), os motores param sozinhos.
  - Ao desconectar, o ESP volta a anunciar para o app conseguir reconectar.

  Não precisa instalar biblioteca: o BLE já vem no pacote "esp32" da Arduino IDE.
  IMPORTANTE: o nome e os UUIDs abaixo precisam ser iguais aos do app (src/features/Joystick/joystick.ble.js).
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// ================== CONFIGURAÇÃO ==================
#define NOME_BLE      "Empilhadeira"
#define SERVICO_UUID  "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define COMANDO_UUID  "6e400002-b5a3-f393-e0a9-e50e24dcca9e"

// Pinos do L298N  >>> TROQUE PELOS PINOS QUE VOCÊ USA <<<
// Tire os jumpers de ENA e ENB do L298N para o controle de velocidade funcionar.
const int ENA = 5, IN1 = 6, IN2 = 7;   // motor esquerdo
const int ENB = 10, IN3 = 8, IN4 = 9;   // motor direito

const int PWM_MIN = 110;   // abaixo disso o motor não sai do lugar; ajuste se precisar
const int PWM_MAX = 255;   // velocidade máxima (reduza para deixar mais lenta)
const unsigned long TIMEOUT_MS = 500;   // para os motores se ficar sem comando
// ==================================================

// Último comando recebido (escrito pelo Bluetooth, lido no loop)
volatile int cmdX = 0, cmdY = 0;
volatile unsigned long ultimoComando = 0;
volatile bool conectado = false;
bool estavaConectado = false;

// v vai de -1 (ré total) a 1 (frente total)
void motor(int en, int a, int b, float v) {
  if (fabs(v) < 0.08) {          // zona morta: parado
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
    analogWrite(en, 0);
    return;
  }
  digitalWrite(a, v > 0 ? HIGH : LOW);
  digitalWrite(b, v > 0 ? LOW : HIGH);
  analogWrite(en, PWM_MIN + (int)((PWM_MAX - PWM_MIN) * fabs(v)));
}

// x: -1 esquerda / 1 direita   y: -1 ré / 1 frente
void dirigir(float x, float y) {
  float esq = constrain(y + x, -1.0f, 1.0f);
  float dir = constrain(y - x, -1.0f, 1.0f);
  motor(ENA, IN1, IN2, esq);
  motor(ENB, IN3, IN4, dir);
}

class ServidorCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* servidor) {
    conectado = true;
  }

  void onDisconnect(BLEServer* servidor) {
    conectado = false;
    cmdX = 0;
    cmdY = 0;
  }
};

class ComandoCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* caracteristica) {
    String valor = caracteristica->getValue().c_str();

    // A elevação (3º valor) é ignorada por enquanto: não há motor de garfo
    int x = 0, y = 0;
    if (sscanf(valor.c_str(), "%d,%d", &x, &y) == 2) {
      cmdX = constrain(x, -100, 100);
      cmdY = constrain(y, -100, 100);
      ultimoComando = millis();
    }
  }
};

void setup() {
  Serial.begin(115200);

  int pinos[] = {ENA, IN1, IN2, ENB, IN3, IN4};
  for (int p : pinos) { pinMode(p, OUTPUT); digitalWrite(p, LOW); }

  BLEDevice::init(NOME_BLE);

  BLEServer* servidor = BLEDevice::createServer();
  servidor->setCallbacks(new ServidorCallbacks());

  BLEService* servico = servidor->createService(SERVICO_UUID);
  BLECharacteristic* comando = servico->createCharacteristic(
    COMANDO_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  comando->setCallbacks(new ComandoCallbacks());
  servico->start();

  BLEDevice::getAdvertising()->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("Bluetooth pronto: procure \"" NOME_BLE "\" pelo app");
}

void loop() {
  // Segurança: sem comando recente, para tudo
  if (millis() - ultimoComando > TIMEOUT_MS) {
    dirigir(0, 0);
  } else {
    dirigir(cmdX / 100.0f, cmdY / 100.0f);
  }

  // Desconectou: volta a anunciar para o app reconectar
  if (!conectado && estavaConectado) {
    delay(300);
    BLEDevice::startAdvertising();
    Serial.println("Desconectado - anunciando de novo");
  }
  if (conectado && !estavaConectado) {
    Serial.println("App conectado");
  }
  estavaConectado = conectado;

  delay(10);

  // >>> Aqui pode entrar o envio de dados para a API, sem usar delay() longo <<<
}

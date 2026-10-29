
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// ============================================
// 1. CONFIGURACOES BLUETOOTH
// ============================================

#define BLE_NAME "EMPILHADEIRA_ESP32"

#define SERVICE_UUID \
  "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"

#define CHARACTERISTIC_UUID \
  "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer* bleServer = nullptr;

volatile bool bluetoothConectado = false;
volatile bool reiniciarBluetooth = false;

// ============================================
// 2. PINOS L298N
// ============================================

// Motor esquerdo
#define ENA 5
#define IN1 6
#define IN2 7

// Motor direito
#define ENB 10
#define IN3 8
#define IN4 9

// ============================================
// 3. CONFIGURACOES DOS MOTORES
// ============================================

// PWM: 0 a 255
const int PWM_MAX = 160;

// Frequencia PWM
const int PWM_FREQ = 1000;

// Resolucao PWM (8 bits)
const int PWM_RESOLUTION = 8;

// Zona morta do joystick (%)
const int DEAD_ZONE = 8;

// Inverte a direcao caso necessario
const bool INVERTER_ESQUERDO = false;
const bool INVERTER_DIREITO = false;

// Ultimas velocidades aplicadas
int velocidadeAnteriorEsq = 0;
int velocidadeAnteriorDir = 0;

// ============================================
// 4. VARIAVEIS DO JOYSTICK
// ============================================

// Valores entre -100 e 100
volatile int joystickX = 0;
volatile int joystickY = 0;

// Tempo do ultimo comando valido
volatile unsigned long ultimoComando = 0;

// Parada automatica apos 450 ms
const unsigned long TIMEOUT_COMANDO = 450;

// ============================================
// 5. CONTROLE DOS MOTORES
// ============================================

// velocidade: -100 a 100
void controlarMotor(
  int enable,
  int inA,
  int inB,
  int velocidade
) {
  velocidade = constrain(velocidade, -100, 100);

  // Desativa o PWM antes de alterar a direcao
  ledcWrite(enable, 0);

  if (velocidade > 0) {
    // Frente
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);

  } else if (velocidade < 0) {
    // Re
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);

  } else {
    // Parado
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
  }

  int pwm = map(
    abs(velocidade),
    0, 100,
    0, PWM_MAX
  );

  ledcWrite(enable, pwm);
}

// ============================================
// 6. PARAR MOTORES
// ============================================

void pararMotores() {
  // Desabilita os motores
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);

  // Coloca as entradas em nivel baixo
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  velocidadeAnteriorEsq = 0;
  velocidadeAnteriorDir = 0;
}

// ============================================
// 7. CONTROLE PELO JOYSTICK
// ============================================

void controlarJoystick(int x, int y) {
  x = constrain(x, -100, 100);
  y = constrain(y, -100, 100);

  // Zona morta para evitar movimentos
  // involuntarios proximo do centro
  if (abs(x) <= DEAD_ZONE) x = 0;
  if (abs(y) <= DEAD_ZONE) y = 0;

  // Mistura diferencial:
  // Y controla frente/re
  // X controla direcao
  int velocidadeEsq = y + x;
  int velocidadeDir = y - x;

  // Normaliza para preservar a proporcao
  // entre os dois motores
  int maior = max(
    100,
    max(abs(velocidadeEsq), abs(velocidadeDir))
  );

  velocidadeEsq = velocidadeEsq * 100 / maior;
  velocidadeDir = velocidadeDir * 100 / maior;

  // Inversao opcional dos motores
  if (INVERTER_ESQUERDO) {
    velocidadeEsq = -velocidadeEsq;
  }

  if (INVERTER_DIREITO) {
    velocidadeDir = -velocidadeDir;
  }

  // Atualiza apenas quando a velocidade muda
  if (velocidadeEsq != velocidadeAnteriorEsq) {
    controlarMotor(
      ENA, IN1, IN2,
      velocidadeEsq
    );

    velocidadeAnteriorEsq = velocidadeEsq;
  }

  if (velocidadeDir != velocidadeAnteriorDir) {
    controlarMotor(
      ENB, IN3, IN4,
      velocidadeDir
    );

    velocidadeAnteriorDir = velocidadeDir;
  }
}

// ============================================
// 8. EVENTOS DA CONEXAO BLE
// ============================================

class BluetoothCallbacks : public BLEServerCallbacks {

  void onConnect(BLEServer* server) override {
    joystickX = 0;
    joystickY = 0;
    ultimoComando = 0;

    bluetoothConectado = true;
    reiniciarBluetooth = false;

    Serial.println("[BLE] Celular conectado!");
  }

  void onDisconnect(BLEServer* server) override {
    bluetoothConectado = false;

    joystickX = 0;
    joystickY = 0;
    ultimoComando = 0;

    reiniciarBluetooth = true;

    Serial.println("[BLE] Celular desconectado!");
  }
};

// ============================================
// 9. RECEBER COMANDOS DO APP
// ============================================

class ComandoCallbacks : public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic* characteristic) override {

    if (!bluetoothConectado) return;

    String comando = characteristic->getValue();

    int x = 0;
    int y = 0;
    char caractereExtra;

    // Formato esperado: "X,Y"
    // Exemplo: "20,80"
    int resultado = sscanf(
      comando.c_str(),
      " %d , %d %c",
      &x,
      &y,
      &caractereExtra
    );

    // Rejeita formatos incorretos e
    // valores fora do intervalo
    if (resultado != 2 ||
        x < -100 || x > 100 ||
        y < -100 || y > 100) {
      Serial.println("[BLE] Comando invalido");
      return;
    }

    joystickX = x;
    joystickY = y;
    ultimoComando = millis();

    Serial.printf(
      "[JOYSTICK] X: %d | Y: %d\n",
      x, y
    );
  }
};

// ============================================
// 10. CONFIGURAR BLUETOOTH
// ============================================

void iniciarBluetooth() {
  BLEDevice::init(BLE_NAME);

  bleServer = BLEDevice::createServer();

  bleServer->setCallbacks(
    new BluetoothCallbacks()
  );

  BLEService* service = bleServer->createService(
    SERVICE_UUID
  );

  BLECharacteristic* characteristic =
    service->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE |
      BLECharacteristic::PROPERTY_WRITE_NR
    );

  characteristic->setCallbacks(
    new ComandoCallbacks()
  );

  service->start();

  BLEAdvertising* advertising =
    BLEDevice::getAdvertising();

  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->start();

  Serial.println("[BLE] Bluetooth iniciado!");
  Serial.println("[BLE] Nome: " BLE_NAME);
}

// ============================================
// 11. SETUP
// ============================================

void setup() {
  Serial.begin(115200);

  // Configura direcao dos motores
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Mantem os motores desabilitados
  // durante a inicializacao
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Configura PWM nos enables
  bool pwmEsq = ledcAttach(
    ENA,
    PWM_FREQ,
    PWM_RESOLUTION
  );

  bool pwmDir = ledcAttach(
    ENB,
    PWM_FREQ,
    PWM_RESOLUTION
  );

  if (!pwmEsq || !pwmDir) {
    Serial.println("[ERRO] Falha ao iniciar PWM");

    while (true) {
      delay(1000);
    }
  }

  pararMotores();

  Serial.println("==========================");
  Serial.println(" EMPILHADEIRA ESP32-C3");
  Serial.println("==========================");

  iniciarBluetooth();

  Serial.println("[SISTEMA] Aguardando conexao...");
}

// ============================================
// 12. LOOP PRINCIPAL
// ============================================

void loop() {

  // Reinicia anuncio apos desconexao
  if (reiniciarBluetooth && !bluetoothConectado) {
    reiniciarBluetooth = false;

    delay(100);
    bleServer->startAdvertising();

    Serial.println("[BLE] Aguardando reconexao...");
  }

  // Sem Bluetooth: motores parados
  if (!bluetoothConectado) {
    pararMotores();
    delay(10);
    return;
  }

  unsigned long agora = millis();

  // Sem comandos recentes: parada automatica
  if (ultimoComando == 0 ||
      agora - ultimoComando > TIMEOUT_COMANDO) {

    pararMotores();

  } else {

    // Executa o comando do joystick
    controlarJoystick(
      joystickX,
      joystickY
    );
  }

  delay(10);
}

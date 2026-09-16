// Definição dos pinos no ESP32
const int pinoAnalogico = 5; // GPIO 34 (ADC1_CH6)
const int pinoDigital = 0;    // GPIO 25
const int pinoLED = 2;         // LED azul nativo na maioria das placas ESP32

void setup() {
  // Inicializa a comunicação serial a 115200 bps (padrão do ESP32)
  Serial.begin(115200);
  
  // Configuração dos pinos
  pinMode(pinoAnalogico, INPUT);
  pinMode(pinoDigital, INPUT);
  pinMode(pinoLED, OUTPUT);
  
  Serial.println("\n--- Teste do Sensor de Luz MH com ESP32 Inicializado ---");
}

void loop() {
  // 1. Leitura Analógica (No ESP32 varia de 0 a 4095)
  // Quanto MENOR o número, MAIS LUZ o sensor recebe
  int valorAnalogico = analogRead(pinoAnalogico);
  
  // 2. Leitura Digital (0 ou 1)
  int valorDigital = digitalRead(pinoDigital);
  
  // Exibe os dados no Monitor Serial
  Serial.print("Luz (Analógico ESP32): ");
  Serial.print(valorAnalogico);
  Serial.print(" | Limiar (Digital): ");
  
  if (valorDigital == HIGH) {
    Serial.println("ESCURO");
    digitalWrite(pinoLED, HIGH); // Liga o LED azul do ESP32
  } else {
    Serial.println("CLARO");
    digitalWrite(pinoLED, LOW);  // Desliga o LED azul
  }
  
  // Aguarda 500 milissegundos antes da próxima leitura
  delay(500);
}

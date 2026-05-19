// =====================================================
//   ROBOT MINISUMO - Arduino Pro Micro
//   Componentes: QTR-3A | KY-022 | TB6612FNG | N20
//   Lógica: esperar señal IR → delay 5s → avanzar
//         + detección de borde (QTR-3A)
// =====================================================

#include <IRremote.h>

// ── PINES TB6612FNG ──────────────────────────────────
// Motor Izquierdo
#define AIN1  4
#define AIN2  5
#define PWMA  9   // PWM

// Motor Derecho
#define BIN1  6
#define BIN2  7
#define PWMB  10  // PWM

#define STBY  8   // Standby del TB6612FNG (HIGH = activo)

// ── PINES SENSORES QTR-3A ────────────────────────────
// Sensor izquierdo, centro, derecho
#define S_IZQ   A0
#define S_CEN   A1
#define S_DER   A2

// ── PINES IR (KY-022) ────────────────────────────────
// Se usan 2 receptores IR; puedes usar uno o ambos
// IRremote solo necesita un pin a la vez en modo básico
#define IR_PIN  2   // receptor IR principal (interrupción)

// ── CONSTANTES ───────────────────────────────────────
#define VELOCIDAD        200   // 0-255
#define UMBRAL_BORDE     500   // valor analógico (blanco < umbral)
#define DELAY_ARRANQUE   5000  // 5 segundos

// Código del botón "play" de tu control remoto
// Cámbialo al valor que ves en el Serial Monitor
#define CODIGO_PLAY      3810328320  // ejemplo NEC; ajusta al tuyo

// ── VARIABLES ────────────────────────────────────────
IRrecv receptor(IR_PIN);
decode_results resultado;

bool robotActivo = false;

// =====================================================
//   SETUP
// =====================================================
void setup() {
  Serial.begin(9600);

  // Pines motor
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Motores apagados al inicio
  digitalWrite(STBY, LOW);

  // Receptor IR
  receptor.enableIRIn();

  Serial.println("Minisumo listo. Esperando señal IR...");
}

// =====================================================
//   LOOP PRINCIPAL
// =====================================================
void loop() {

  // ── 1. Escuchar control remoto ──────────────────
  if (!robotActivo && receptor.decode(&resultado)) {
    Serial.print("Código recibido: 0x");
    Serial.println(resultado.value, HEX);

    if (resultado.value == CODIGO_PLAY) {
      Serial.println("¡Play recibido! Arrancando en 5 segundos...");
      delay(DELAY_ARRANQUE);
      robotActivo = true;
      digitalWrite(STBY, HIGH);  // Habilitar driver
      Serial.println("¡Combate!");
    }

    receptor.resume();  // Listo para siguiente señal
  }

  // ── 2. Lógica de combate ────────────────────────
  if (robotActivo) {
    int valIzq = analogRead(S_IZQ);
    int valCen = analogRead(S_CEN);
    int valDer = analogRead(S_DER);

    bool bordeIzq = (valIzq < UMBRAL_BORDE);
    bool bordeCen = (valCen < UMBRAL_BORDE);
    bool bordeDer = (valDer < UMBRAL_BORDE);

    // ── Reacciones al borde blanco ────────────────
    if (bordeIzq && bordeDer) {
      // Borde al frente → retroceder y girar
      retroceder();
      delay(300);
      girarDerecha();
      delay(400);
    }
    else if (bordeIzq) {
      // Borde izquierdo → retroceder y girar a la derecha
      retroceder();
      delay(200);
      girarDerecha();
      delay(300);
    }
    else if (bordeDer) {
      // Borde derecho → retroceder y girar a la izquierda
      retroceder();
      delay(200);
      girarIzquierda();
      delay(300);
    }
    else {
      // Sin borde → avanzar
      avanzar();
    }
  }
}

// =====================================================
//   FUNCIONES DE MOVIMIENTO
// =====================================================

void avanzar() {
  // Motor A - adelante
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, VELOCIDAD);

  // Motor B - adelante
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, VELOCIDAD);
}

void retroceder() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, VELOCIDAD);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, VELOCIDAD);
}

void girarDerecha() {
  // Motor A adelante, Motor B atrás
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, VELOCIDAD);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, VELOCIDAD);
}

void girarIzquierda() {
  // Motor A atrás, Motor B adelante
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, VELOCIDAD);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, VELOCIDAD);
}

void detener() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  digitalWrite(STBY, LOW);
}

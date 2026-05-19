#include <IRremote.hpp>

/////////////////////////////////////////////////////////
// PIN IR START
/////////////////////////////////////////////////////////

#define IR_RECEIVE_PIN 2
#define PLAY_CODE 3810328320

/////////////////////////////////////////////////////////
// TB6612FNG
/////////////////////////////////////////////////////////

// MOTOR IZQUIERDO
#define PWMA 3
#define AIN1 5
#define AIN2 6

// MOTOR DERECHO
#define BIN1 7
#define BIN2 8
#define PWMB 9

// STANDBY
#define STBY 4

/////////////////////////////////////////////////////////
// QTR / BARRA IR
/////////////////////////////////////////////////////////

#define QTR_LEFT   A0
#define QTR_CENTER A1
#define QTR_RIGHT  A2

/////////////////////////////////////////////////////////
// VARIABLES
/////////////////////////////////////////////////////////

bool iniciado = false;

bool lineaDetectada = false;

int velocidadBusqueda = 180;
int velocidadAtaque = 255;
int velocidadEscape = 220;

/////////////////////////////////////////////////////////
// SETUP
/////////////////////////////////////////////////////////

void setup() {

  Serial.begin(9600);

  configurarMotores();
  configurarSensores();
  configurarControlIR();

  Serial.println("ROBOT LISTO");
}

/////////////////////////////////////////////////////////
// LOOP
/////////////////////////////////////////////////////////

void loop() {

  if (!iniciado) {

    esperarInicio();
  }
  else {

    leerSensores();

    if (lineaDetectada) {

      escapar();
    }
    else {

      buscar();
    }
  }
}

/////////////////////////////////////////////////////////
// IR
/////////////////////////////////////////////////////////

void configurarControlIR() {

  IrReceiver.begin(IR_RECEIVE_PIN);

  Serial.println("IR CONFIGURADO");
}

/////////////////////////////////////////////////////////
// MOTORES
/////////////////////////////////////////////////////////

void configurarMotores() {

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  Serial.println("MOTORES CONFIGURADOS");
}

/////////////////////////////////////////////////////////
// SENSORES
/////////////////////////////////////////////////////////

void configurarSensores() {

  pinMode(QTR_LEFT, INPUT);
  pinMode(QTR_CENTER, INPUT);
  pinMode(QTR_RIGHT, INPUT);

  Serial.println("SENSORES CONFIGURADOS");
}

/////////////////////////////////////////////////////////
// ESPERAR START
/////////////////////////////////////////////////////////

void esperarInicio() {

  Serial.println("ESPERANDO PLAY");

  if (IrReceiver.decode()) {

    unsigned long codigo =
      IrReceiver.decodedIRData.decodedRawData;

    Serial.println(codigo);

    if (codigo == PLAY_CODE) {

      Serial.println("INICIANDO EN 5 SEGUNDOS");

      delay(5000);

      iniciado = true;
    }

    IrReceiver.resume();
  }

  delay(300);
}

/////////////////////////////////////////////////////////
// LEER SENSORES
/////////////////////////////////////////////////////////

void leerSensores() {

  int izquierda = analogRead(QTR_LEFT);
  int centro = analogRead(QTR_CENTER);
  int derecha = analogRead(QTR_RIGHT);

  Serial.print("IZQ: ");
  Serial.print(izquierda);

  Serial.print("  CEN: ");
  Serial.print(centro);

  Serial.print("  DER: ");
  Serial.println(derecha);

  ///////////////////////////////////////////////////////
  // BORDE BLANCO
  ///////////////////////////////////////////////////////

  if (izquierda < 500 ||
      centro < 500 ||
      derecha < 500) {

    lineaDetectada = true;
  }
  else {

    lineaDetectada = false;
  }
}

/////////////////////////////////////////////////////////
// BUSCAR
/////////////////////////////////////////////////////////

void buscar() {

  Serial.println("BUSCANDO");

  girarDerecha();
}

/////////////////////////////////////////////////////////
// ESCAPAR
/////////////////////////////////////////////////////////

void escapar() {

  Serial.println("ESCAPANDO");

  retroceder();

  delay(300);

  girarIzquierda();

  delay(300);
}

/////////////////////////////////////////////////////////
// MOVIMIENTOS
/////////////////////////////////////////////////////////

void avanzar() {

  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, velocidadAtaque);
  analogWrite(PWMB, velocidadAtaque);
}

/////////////////////////////////////////////////////////

void retroceder() {

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, velocidadEscape);
  analogWrite(PWMB, velocidadEscape);
}

/////////////////////////////////////////////////////////

void girarDerecha() {

  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMA, velocidadBusqueda);
  analogWrite(PWMB, velocidadBusqueda);
}

/////////////////////////////////////////////////////////

void girarIzquierda() {

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, velocidadBusqueda);
  analogWrite(PWMB, velocidadBusqueda);
}

/////////////////////////////////////////////////////////

void detener() {

  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

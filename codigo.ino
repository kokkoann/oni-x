#include <IRremote.hpp>


#define IR_RECEIVE_PIN 2
#define PLAY_CODE 3810328320


bool iniciado = false;


void setup() {


  Serial.begin(9600);


  IrReceiver.begin(IR_RECEIVE_PIN);


}


void loop() {


  if (!iniciado) {


    esperarInicio();


  }


  else {


    comportamientoSumo();


  }
}


void esperarInicio() {


  Serial.println("Esperando PLAY");


  if (IrReceiver.decode()) {


    unsigned long codigo =
    IrReceiver.decodedIRData.decodedRawData;


    if (codigo == PLAY_CODE) {


      Serial.println("INICIANDO EN 5 SEGUNDOS");


      delay(5000);


      iniciado = true;
    }


    IrReceiver.resume();
  }
}


void comportamientoSumo() {


  Serial.println("BUSCANDO ENEMIGO");


}


//SI DETECTA NEGRO RETROCEDER



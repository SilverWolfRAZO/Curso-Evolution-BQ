/******************************************************************
 *                           Librerias                            *
 ******************************************************************/ 

#include <Servo.h>

/******************************************************************
 *                    Definicion de variable                      *
 ******************************************************************/

/* Definicion de Pines para la placa */

#define pinRuedaIzquierda              8   
#define pinRuedaDerecha                9   
#define pinSensorLDRIzquierda         A2      /*   Sensor de luz izquierdo     */ 
#define pinSensorLDRDerecha           A3      /*   Sensor de luz derecho       */

/* Definicion de los valores que se pones a los servos de rotación continua,
 es decir, a las ruedas */
#define ruedaParadaValor              90
#define ruedaIzquierdaAvanceValor      0
#define ruedaIzquierdaRetrocesoValor 180
#define ruedaDerechaAvanceValor      180
#define ruedaDerechaRetrocesoValor     0

/* Definimos los posibles estados de la maquina de estados del programa */
#define CONTROL_MANUAL                 0
#define SEGUIR_LINEAS                  1
#define EVITAR_LUCES                   2
#define EVITAR_OBSTACULOS              3

/* Variable que controla el estado actual del programa */
int estadoActual;

/* Objetos de la clase servo para cada servo */
Servo ruedaIzquierda;                         /*  Valores entre 0 y 180  */
Servo ruedaDerecha;                           /*  Valores entre 0 y 180  */

/* Variables del modo evitar luz */
int sensorLuzIzquierda;
int sensorLuzDerecha;
int luzValorLimite;

/* Delay por defecto */
#define delayDefecto                 10

/******************************************************************
 *                     Definicion de funciones                    *
 ******************************************************************/

void pararRuedas() {
  ruedaIzquierda.write(ruedaParadaValor);
  delay(delayDefecto);

  ruedaDerecha.write(ruedaParadaValor);
  delay(delayDefecto);
}

/* El robot esta en el modo evitar la luz */
void evitarLaLuz() {
  /* Leer los datos de los sensores LDR */
  sensorLuzIzquierda = analogRead(pinSensorLDRIzquierda);
  sensorLuzDerecha = analogRead(pinSensorLDRDerecha);
  
  /* Si el sensor de la derecha no recibe nada pararemos la rueda derecha,
  sino haremos que avance */
  if (sensorLuzDerecha < luzValorLimite) {
    ruedaDerecha.write(ruedaParadaValor);
    delay(delayDefecto);

  } else {
    ruedaDerecha.write(ruedaDerechaAvanceValor);
    delay(delayDefecto);
  }
  
  /* Si el sensor de la izquierda no recibe nada pararemos la rueda izquierda,
  sino haremos que avance */
  if (sensorLuzIzquierda < luzValorLimite) {
    ruedaIzquierda.write(ruedaParadaValor);
    delay(delayDefecto);

  } else {
    ruedaIzquierda.write(ruedaIzquierdaAvanceValor);
    delay(delayDefecto);

  }
}

/******************************************************************
 *                             Setup                              *
 ******************************************************************/
 
void setup() {
  /* Definimos el pin necesario para cada objeto */
  ruedaIzquierda.attach(pinRuedaIzquierda);
  ruedaDerecha.attach(pinRuedaDerecha);

  /* No definimos los pines para los sensores analogiocs del A0 y A1 */

  /* Paramos el robot al principio */
  pararRuedas();

  /* Actualizamos el estado */
  estadoActual = EVITAR_LUCES;

  /*Elegimos un valor umbral para la luz */
  luzValorLimite = 200;
}

/******************************************************************
 *                             Loop                              *
 ******************************************************************/
 
void loop() {

  /* Segun el estado en el que esté realizo una u otra tarea */
  switch (estadoActual)
  {
    case CONTROL_MANUAL:

      break;
    case SEGUIR_LINEAS:

      break;
    case EVITAR_LUCES:
      evitarLaLuz();
      break;
    case EVITAR_OBSTACULOS:
      
      break;
  }
  
}

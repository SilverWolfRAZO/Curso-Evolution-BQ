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
#define pinCabeza                     11
#define pinUSTri                       4   /*   Sensor ultrasonico trigger */
#define pinUSEch                       5   /*   Sensor ultrasonico echo    */
  
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

/* Variables del modo evitar obstaculos */
#define US_ANGULO_CENTRO                     80  
#define US_ANGULO_IZQUIERDA                 110
#define US_ANGULO_DERECHA                    50  
#define US_DELAY_ESPERA_RESPUESTA           500     
#define OBSTACULO_DETECTADO                   0
#define OBSTACULO_NO_DETECTADO                1
#define DELAY_BUSCAR_OBSTACULO_CABEZA       500
int obstaculo = OBSTACULO_NO_DETECTADO;
Servo cabeza;                                 /*  Valores entre 0 y 180  */
boolean ibaHaciaDelante = true;
int ultimoAnguloCabeza; 

/* El modo de evitar obstaculo consume mucho tiempo, durante el cual
   los datos recividos por el Bluetooth no se esten tratando. Para
   solucionar esto se ha divido el algoritmo en varias partes de forma
   que entre las diferentes etapas se procesen los datos recibidos */
#define US_STATE_CHECK_NO_EMPEZADO          0   
#define US_STATE_CHECK_CENTRO               1
#define US_STATE_CHECK_DERECHA              2
#define US_STATE_CHECK_IZQUIERDA            3

int ultimoEstadoUS = US_STATE_CHECK_NO_EMPEZADO;
void (*pt2Funcion)() = NULL; 

/* Delay por defecto */
#define delayDefecto                 10

/******************************************************************
 *                     Definicion de funciones                    *
 ******************************************************************/

//bqBAT (bq US)
long TP_init_4_5() {
  digitalWrite(pinUSTri, LOW);
  delayMicroseconds(2);
  digitalWrite(pinUSTri, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinUSTri, LOW);
  long microsegundos = pulseIn(pinUSEch ,HIGH);
  return microsegundos;
}

long distancia_4_5() {
  long microsegundos = TP_init_4_5();
  long distancia;
  distancia = microsegundos/29/2;
  return distancia;
}


void girarCabeza(int angulo, int duracionDelay) {
  cabeza.write(angulo);
  delay(duracionDelay);
  ultimoAnguloCabeza = angulo;
}

int buscarObstaculo(int angulo) {
  int distancia = 0;

  if(angulo != ultimoAnguloCabeza)
  {
    girarCabeza(angulo, DELAY_BUSCAR_OBSTACULO_CABEZA);
  }

  distancia = distancia_4_5();

  if ((distancia != 0) && (distancia < 25)) {
    return OBSTACULO_DETECTADO;
    
  } 

  return OBSTACULO_NO_DETECTADO;
}

void pararRuedas() {
  ruedaIzquierda.write(ruedaParadaValor);
  delay(delayDefecto);

  ruedaDerecha.write(ruedaParadaValor);
  delay(delayDefecto);
}

void const irAdelante() {
  if(ultimoAnguloCabeza != US_ANGULO_CENTRO) {
    girarCabeza(US_ANGULO_CENTRO, delayDefecto);
  }
  
  ruedaIzquierda.write(ruedaIzquierdaAvanceValor);
  delay(delayDefecto);

  ruedaDerecha.write(ruedaDerechaAvanceValor);
  delay(delayDefecto);
}


void irAtras() {
  ruedaIzquierda.write(ruedaIzquierdaRetrocesoValor);
  delay(delayDefecto);

  ruedaDerecha.write(ruedaDerechaRetrocesoValor);
  delay(delayDefecto);
}


void irIzquierda() {
  ruedaIzquierda.write(ruedaParadaValor);
  delay(delayDefecto);

  ruedaDerecha.write(ruedaDerechaAvanceValor);
  delay(delayDefecto);
}


void irDerecha() {
  ruedaIzquierda.write(ruedaIzquierdaAvanceValor);
  delay(delayDefecto);

  ruedaDerecha.write(ruedaParadaValor);
  delay(delayDefecto);
}

void buscarObjeto(int angulo, int nuevoEstado, void (*pt2Funcion)()){
    obstaculo = buscarObstaculo(angulo);
    delay(DELAY_BUSCAR_OBSTACULO_CABEZA);

    if (obstaculo == OBSTACULO_NO_DETECTADO) {
      (*pt2Funcion)();
      ultimoEstadoUS = US_STATE_CHECK_NO_EMPEZADO;
    } else {
      ultimoEstadoUS = nuevoEstado;
    }
}

/* El robot esta en modo evitar obstaculos */
void evitarObstaculos() {
  switch (ultimoEstadoUS) {
      /* No he compradobado ninguna posicion */
      case US_STATE_CHECK_NO_EMPEZADO:
        pt2Funcion = &irAdelante;
        buscarObjeto(US_ANGULO_CENTRO,US_STATE_CHECK_CENTRO,pt2Funcion);
        break;
      /* Hay un objeto delante mia */
      case US_STATE_CHECK_CENTRO:
        pt2Funcion = &irDerecha;
        buscarObjeto(US_ANGULO_DERECHA,US_STATE_CHECK_DERECHA,pt2Funcion);
        break;
      /* Hay un objeto delante mia y a mi derecha */
      case US_STATE_CHECK_DERECHA:
        pt2Funcion = &irIzquierda;
        buscarObjeto(US_ANGULO_IZQUIERDA,US_STATE_CHECK_IZQUIERDA,pt2Funcion);
        break;
      /* Hay un objeto delate mia, a mi derecha y a mi izquierda */
      case US_STATE_CHECK_IZQUIERDA:
        ultimoEstadoUS = US_STATE_CHECK_CENTRO; 
        irAtras();
        break;
  }
}

/******************************************************************
 *                             Setup                              *
 ******************************************************************/

void setup() {
  
  /* Definimos el pin necesario para cada objeto */
  ruedaIzquierda.attach(pinRuedaIzquierda);
  ruedaDerecha.attach(pinRuedaDerecha);
  cabeza.attach(pinCabeza);
  /* No definimos los pines para los sensores analogiocs del A0 y A1 */

  /* Paramos el robot al principio */
  pararRuedas();

  /* Miramos al frente */
  girarCabeza(US_ANGULO_CENTRO,DELAY_BUSCAR_OBSTACULO_CABEZA);
  
  /* Actualizamos el estado */
  estadoActual = EVITAR_OBSTACULOS;

  /* Actualizamos el estado US */
  ultimoEstadoUS = US_STATE_CHECK_NO_EMPEZADO;
}

/******************************************************************
 *                  Principal loop del programa                   *
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
    
      break;
    case EVITAR_OBSTACULOS:
      evitarObstaculos();
      break;
  }
}

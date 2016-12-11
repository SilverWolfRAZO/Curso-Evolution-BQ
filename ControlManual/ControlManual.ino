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

/* Definicion de los valores que se pones a los servos de rotación continua,
 es decir, a las ruedas */
#define ruedaParadaValor              90
#define ruedaIzquierdaAvanceValor      0
#define ruedaIzquierdaRetrocesoValor 180
#define ruedaDerechaAvanceValor      180
#define ruedaDerechaRetrocesoValor     0

/* Baudios para el Bluetooth*/
#define BQ_ZUM_BLUETOOTH                       19200

/* Tamaño del buffer de datos recibidos */
#define tamanoBuffer 5

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

/* Delay por defecto */
#define delayDefecto                 10

/*  Un array de caracteres para almanecenar los datos recibidos del Serial del Bluetooth  */
 
char datosBuffer[tamanoBuffer]; 

/* iterador del Buffer */
int i = 0;

/* Cantidad de caracteres disponibles en el Serial */
int numChar = 0;    

/******************************************************************
 *                     Definicion de funciones                    *
 ******************************************************************/
 
void pararRuedas() {
  ruedaIzquierda.write(ruedaParadaValor);
  delay(delayDefecto);

  ruedaDerecha.write(ruedaParadaValor);
  delay(delayDefecto);
}

void irAdelante() {
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

/* Administracion del buffer de datos */
void checkDatos(char* datos){  
  
   switch(datos[0]) {

    /* Boton de Control Manual presionado */
    case 'M':
      estadoActual = CONTROL_MANUAL;
      pararRuedas();
      break;

    /* Boton de Parar pulsado */
    case'S':
      pararRuedas();
      break;

    /* Boton de ir hacia delante pulsado */
    case 'U':
      irAdelante();
      break;

    /* Boton de ir hacia dentras pulsado */
    case 'D':
      irAtras();
      break;

    /* Boton de ir hacia la izquierda pulsado */ 
    case 'L':
      irIzquierda();
      break;

    /* Boton de iz hacia la derecha pulsado */ 
    case 'R':
      irDerecha();
      break;
    
  } 

  /* Vaciar el Serial */   
  Serial.flush();

}

/******************************************************************
 *                             Setup                              *
 ******************************************************************/

void setup() {
  /* Abrimos la conexión Serial y la vaciamos*/
  Serial.begin(BQ_ZUM_BLUETOOTH);  
  Serial.flush(); 
  
  /* Definimos el pin necesario para cada objeto */
  ruedaIzquierda.attach(pinRuedaIzquierda);
  ruedaDerecha.attach(pinRuedaDerecha);

  /* No definimos los pines para los sensores analogiocs del A0 y A1 */

  /* Paramos el robot al principio */
  pararRuedas();

  /* Actualizamos el estado */
  estadoActual = CONTROL_MANUAL;
}

/******************************************************************
 *                  Principal loop del programa                   *
 ******************************************************************/
 
void loop() {
  /* Si hay algo en el puerto Serie del Bluetooth */
  if (Serial.available() > 0) { 
   
    /* Reiniciamos el iterador y limpiamos el buffer */
    i = 0;
    memset(datosBuffer, 0, sizeof(datosBuffer));  
    
    /* Emperamos a que el buffer se llene. Depente del tamaño de la
     *  informacion, 1 ms por cada caracter mas o menos */
    delay(tamanoBuffer); 

    /* Numero de caracteres disponibles en el puerto Serie del Bluetooth */
    numChar = Serial.available();   
    
    /* Limitamos el numero de caracteres que se van a leer del Serial
     * evitando leer mas que el tamaño del buffer */
    if (numChar > tamanoBuffer) {
          numChar = tamanoBuffer;
    }

    /* Leemos el puerto Serie del Bluetooth y lo guardamos en el buffer */
    while (numChar--) {
        datosBuffer[i++] = Serial.read();

        /* El microcontrolador es más rapido adquiriendo datos que el puerto Serie
        proporcionandolos, por lo que este delay asegura el buen funcionamiento
        de la lectura y que no aparezcan arrays vacios*/
        delay(3);
    } 

    /* Administracion del buffer de datos */   
    checkDatos(datosBuffer);
    
  }
  
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
      
      break;
  }
}

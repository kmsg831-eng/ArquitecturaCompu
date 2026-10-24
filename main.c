#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#define TIPO_VEHICULAR  0
#define TIPO_PEATONAL   1
#define VERDE       0
#define PARPADEO    1
#define ROJO        2
#define APAGADO     3
#define PUERTO_B    0
#define PUERTO_C    1
#define PUERTO_D    2
#define MODO_NORMAL       0
#define MODO_HORA_PICO    1
#define MODO_BAJO_TRAFICO 2
//PROGREM=Memoria flash
//---TABLAS---//
const uint8_t EntradasPuerto[] PROGMEM = {
    PUERTO_D,PUERTO_D,PUERTO_D
};
/*en que puerto y pin esta cada entrada. 
El indice 0 es el boton, el 1 y el 2 son los switches de modo.*/
const uint8_t EntradasPin[] PROGMEM = {
    PD0,PD1,PD2
};

#define NUM_ENTRADAS  (sizeof(EntradasPin) / sizeof(EntradasPin[0]))/*se calculan con sizeof que da el numero de elementos. 
Si agregas un estado a la tabla, el codigo se ajusta solo.*/

//Orden: verde A, rojo A, verde B, rojo B, peaton 
const uint8_t SalidasPuerto[] PROGMEM = {
    PUERTO_C,PUERTO_C,PUERTO_C,PUERTO_C,PUERTO_C
};
/*lo mismo para las 5 salidas, en este orden:
 verde A, rojo A, verde B, rojo B, peaton.*/
const uint8_t SalidasPin[] PROGMEM = {
    PC1,PC2,PC3,PC4,PC5   
};

const uint8_t FuncionLuz[] PROGMEM = {//el color de cada salida
    VERDE,ROJO,VERDE,ROJO,VERDE
};

#define NUM_SALIDAS  (sizeof(SalidasPin) / sizeof(SalidasPin[0]))/*se calculan con sizeof que da el numero de elementos. 
Si agregas un estado a la tabla, el codigo se ajusta solo.*/

const uint8_t Tipo[] PROGMEM = {// si es peatonal o normal
    TIPO_VEHICULAR,TIPO_VEHICULAR,TIPO_PEATONAL
};
#define NUM_SEMAFOROS  (sizeof(Tipo) / sizeof(Tipo[0]))/*se calculan con sizeof que da el numero de elementos. 
Si agregas un estado a la tabla, el codigo se ajusta solo.*/

//primera salida de cada semaforo 
/*los indices son las salidas(inicio)*/
const uint8_t InicioLuz[] PROGMEM = {
    0,2,4   
};

// cuantos LEDs por semaforo(cantidas)
const uint8_t NumLuces[] PROGMEM = {
    2,2,1
};

//estados
const uint8_t Estados[][NUM_SEMAFOROS] PROGMEM = {//sem A. semB y sem C abajo s0...
    {VERDE,     ROJO,     APAGADO}, // S0
    {PARPADEO,  ROJO,     APAGADO}, // S1
    {ROJO,      VERDE,    APAGADO}, // S2
    {ROJO,      PARPADEO, APAGADO}, // S3
    {ROJO,      ROJO,     VERDE}    // S4
};

#define NUM_ESTADOS (sizeof(Estados) / sizeof(Estados[0]))/*se calculan con sizeof que da el numero de elementos. 
Si agregas un estado a la tabla, el codigo se ajusta solo.*/
#define NUM_TRANSICIONES NUM_ESTADOS

const uint16_t trafico[] PROGMEM = {//tiempo del trafico
    1000, 500, 2000   
};

const uint8_t trans[NUM_TRANSICIONES][2] PROGMEM = {
    {1, 1}, // S0 -> S1  siempre va a S1
    {2, 2}, // S1 -> S2
    {3, 3}, // S2 -> S3
    {0, 4}, // S4 -> S0 o S5 sin pedir va a S0, con pedir va a S5
    {0, 0}  // S5 -> S0 va a s0
};

// configurar pin 
/*DDRx =regristros bit en 1 = s,0=e
PORTx: en un pin de salida encedido;entrada un 1 activa el pull-up interno.
PINx: lee el estado real del pin*/
void configurarPin(uint8_t puerto, uint8_t pin,uint8_t esSalida){
    switch (puerto) {
        case PUERTO_B:
            if (esSalida) {//si es salida
                DDRB |= (1 << pin);//como salida
                PORTB &= ~(1 << pin);//apagado incio
            } else {//si no es entrada
                DDRB &= ~(1 << pin);//como entrada
                PORTB |= (1 << pin);//pullup
            }
            break;
        case PUERTO_C:
            if (esSalida) {
                DDRC |= (1 << pin);
                PORTC &= ~(1 << pin);
            } else {
                DDRC &= ~(1 << pin);
                PORTC |= (1 << pin);
            }
            break;
        case PUERTO_D:
            if (esSalida) {
                DDRD |= (1 << pin);
                PORTD &= ~(1 << pin);
            } else {
                DDRD &= ~(1 << pin);
                PORTD |= (1 << pin);
            }
            break;
    }
}

void entradas(void){
    for (uint8_t i = 0; i < NUM_ENTRADAS; i++) {
        uint8_t puerto = pgm_read_byte(&EntradasPuerto[i]);
        uint8_t pin = pgm_read_byte(&EntradasPin[i]);
        configurarPin(puerto, pin, 0);
    }
}
/*Recorren las tablas de pines y llaman a configurarPin para cada uno.*/
void salidas(void)
{
    for (uint8_t i = 0; i < NUM_SALIDAS; i++) {
        uint8_t puerto = pgm_read_byte(&SalidasPuerto[i]);
        uint8_t pin = pgm_read_byte(&SalidasPin[i]);
        configurarPin(puerto, pin, 1);
    }
}

// Prender o apagar un pin de salida (portx |= o &=)
void escribirPin(uint8_t puerto, uint8_t pin,uint8_t valor){
    /* Se usa switch porque el puerto viene de una tabla 
    (el num) y hay que elegir el registro fisico.*/
    switch (puerto) {
        case PUERTO_B:
            if (valor)
                PORTB |= (1 << pin);
            else
                PORTB &= ~(1 << pin);
            break;
        case PUERTO_C:
            if (valor)
                PORTC |= (1 << pin);
            else
                PORTC &= ~(1 << pin);
            break;
        case PUERTO_D:
            if (valor)
                PORTD |= (1 << pin);
            else
                PORTD &= ~(1 << pin);
            break;
    }
}

// leer dip devuelve (PINx & (1<<pin)) != 0, es decir 1 o 0.
uint8_t leerPin(uint8_t puerto, uint8_t pin){
    switch (puerto) {
        case PUERTO_B:
            return (PINB & (1 << pin)) != 0;
        case PUERTO_C:
            return (PINC & (1 << pin)) != 0;
        case PUERTO_D:
            return (PIND & (1 << pin)) != 0;
        default:
            return 0;
    }
}
/*usan en las tablas de entradas el puerto y pin del indice dado y llama a leerPin*/
uint8_t leer(uint8_t indice){
    uint8_t puerto =pgm_read_byte(&EntradasPuerto[indice]);
    uint8_t pin = pgm_read_byte(&EntradasPin[indice]);
    return leerPin(puerto, pin);
}

// Deccide que led encender en cada semaforo 
/*Recorre los LEDs de ese semaforo (de inicio a inicio + cantidad). 
Para cada LED, lee su funcion (verde o rojo) y aplica esta logica:
Estado VERDE y LED verde enciende.
Estado ROJO y LED rojo enciende.
Estado PARPADEO y LED verde y verdeParpadeando = 1 enciende.
Cualquier otro caso (incluido APAGADO)  apaga.*/

void contrSemaforo(uint8_t semaforo,uint8_t estado, uint8_t verdeParpadeando) {
    uint8_t inicio = pgm_read_byte(&InicioLuz[semaforo]);
    uint8_t cantidad = pgm_read_byte(&NumLuces[semaforo]);

    for (uint8_t j = 0; j < cantidad; j++) {
        uint8_t indice = inicio + j;
        uint8_t puerto = pgm_read_byte(&SalidasPuerto[indice]);
        uint8_t pin =  pgm_read_byte(&SalidasPin[indice]);
        uint8_t funcion = pgm_read_byte(&FuncionLuz[indice]);
        uint8_t encender = 0;

        if (estado == VERDE && funcion == VERDE) {
            encender = 1;
        }
        else if (estado == ROJO && funcion == ROJO) {
            encender = 1;
        }
        else if (estado == PARPADEO &&funcion == VERDE && verdeParpadeando) {//1 es prendido, 0 es apagado
            encender = 1;
        }
        escribirPin(puerto, pin, encender);
    }
}

//leer la tabla de estados y recorrer cad semaforo 
/*Lee una fila de la tabla Estados y llama a contrSemaforo para 
cada semaforo */
void aplicarEstado(uint8_t est, uint8_t verdeParpadeando){
    for (uint8_t i = 0; i < NUM_SEMAFOROS; i++) {
        uint8_t estado = pgm_read_byte(&Estados[est][i]);
        contrSemaforo(i, estado, verdeParpadeando);
    }
}



// cuanto dura el estado y si estamos en parpadeo 
/*Aplica el estado y espera el tiempo. 
Primero revisa si algun SEMA PARPADEO:*/ 
void ejecutarEstado(uint8_t est, uint16_t tiempo){
    uint8_t hayParpadeo = 0;
    for (uint8_t i = 0; i < NUM_SEMAFOROS; i++) {//for de _delay_ms(1) y no con _delay_ms(tiempo)
        uint8_t estado = pgm_read_byte(&Estados[est][i]);
        if (estado == PARPADEO) {
            hayParpadeo = 1;
            break;
        }
    }

    if (hayParpadeo) {
        uint16_t segmento = tiempo / 4;
        for (uint8_t fase = 0; fase < 4; fase++) {
            aplicarEstado(est, (fase % 2) == 0);
            for (uint16_t ms = 0; ms < segmento; ms++) {
                _delay_ms(1);//. Repetir 1 ms muchas veces
            }
        }
    } 
    else {
        aplicarEstado(est, 0);
        for (uint16_t ms = 0; ms < tiempo; ms++) {
            _delay_ms(1);//_delay_ms() necesita un argumento constante conocido en compilacion
        }
    }
}

// verifica que el siguente estado es de tipo peatonal, devolviendo 1
//
uint8_t peaton(uint8_t est){
    for (uint8_t i = 0; i < NUM_SEMAFOROS; i++) {
        uint8_t tipo = pgm_read_byte(&Tipo[i]);//Revisa los semaforos de tipo TIPO_PEATONAL
        uint8_t estado = pgm_read_byte(&Estados[est][i]);

        if (tipo == TIPO_PEATONAL && estado == VERDE) {//: divide el tiempo en 4 segmentos (tiempo/4
            return 1;
        }
    }//Sirve para saber si el siguiente estado posible es el de peatones.

    return 0;
}

int main(void)
{
    entradas();
    salidas();

    uint8_t est = 0;//salidas del esatdo 0
    uint8_t pea;//vairables 
    uint8_t sig;
    uint8_t modo;
    uint8_t modoBit0;
    uint8_t modoBit1;

    uint16_t tiempo;

    while (1) {

        modoBit0 = leer(1);//llama a funcion
        modoBit1 = leer(2);//llama a funcion

        if (modoBit0 && !modoBit1) {
            modo = MODO_HORA_PICO;//bit0=1, bit1=0 HORA_PICO
        }
        else if (!modoBit0 && modoBit1) {
            modo = MODO_BAJO_TRAFICO;//bit0=0, bit1=1 BAJO_TRAFICO
        }
        else {
            modo = MODO_NORMAL;//cualquier otra combinacion (00 o 11) NORMAL
        }

        // Obtener el tiempo 
        tiempo = pgm_read_word(&trafico[modo]);

        // Ejecutar el estado actual durante ese tiempo
        ejecutarEstado(est, tiempo);
    
        sig=pgm_read_byte(&trans[est][1]);
        // verificar si se solicito peaton
        if(peaton(sig)){
            pea =!leer(0);
        }
        else{
            pea=0;
        } 
        // siguiente
        est = pgm_read_byte(&trans[est][pea]);
    }
    return 0;
}     
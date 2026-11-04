#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Bandera global para avisar al bucle principal que hubo una interrupción
volatile uint8_t boton_interrumpido = 0;
uint8_t estado_anterior = (1 << PIND2);

ISR(INT0_vect) {
    //  Apagar inmediatamente la interrupción para que el AVR se vuelva "sordo" a los rebotes
    EIMSK &= ~(1 << INT0);
    
    //  Levantar la bandera para que la función check_button_debounce() empiece a trabajar
    boton_interrumpido = 1;
}

void check_button_debounce(void) {
    // Si la interrupción no se ha disparado, salimos inmediatamente sin hacer nada
    if (!boton_interrumpido) {
        return;
    }

    // Esperamos 50ms a que los rebotes  terminen 
    // Durante este tiempo la interrupción INT0 sigue APAGADA.
    _delay_ms(50); 

    uint8_t estado_actual = PIND & (1 << PIND2);

    if (estado_anterior != estado_actual) {
        PORTC ^= (1 << PORTC2);
    }
    estado_anterior = estado_actual;

    // Escribir un '1' en el bit INTF0 del registro EIFR borra  señales falsas acumuladas.
    EIFR |= (1 << INTF0); 

    // Bajamos la bandera de control para poder detectar un nuevo clic en el futuro
    boton_interrumpido = 0;

    // Volvemos a encender la interrupción externa INT0
    EIMSK |= (1 << INT0);
}

int main(void) {

    DDRC|=(1<<DDC2);
    DDRD &=~(1<<DDD2);
    PORTD |=(1<<PORTD2);
    EICRA=(0<<ISC01)|(1<<ISC00);
    EIFR=(1<<INTF0);
    EIMSK=(1<<INT0);
    sei(); 

    while (1) {
        // Ejecutar continuamente la función de control
        check_button_debounce();
    }
}




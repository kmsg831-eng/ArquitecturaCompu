#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdint.h>

ISR(INT0_vect){
   PORTC^=(1<<PORTC2);
}

int main(){
    DDRC|=(1<<DDC2);
    DDRD &=~(1<<DDD2);
    PORTD |=(1<<PORTD2);
    EICRA=(1<<ISC01)|(1<<ISC00);
    EIFR=(1<<INTF0);
    EIMSK=(1<<INT0);
    sei();
    while(1){

    }
}
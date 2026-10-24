#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdint.h>

const uint16_t velocidades[]PROGMEM ={
    500,1000,2000,3000,4000,5000,6000,7000
};

//permite cambiar el dalay  segun la combinacion 
void retardo_ms(uint16_t velocidad)
{
    while (velocidad > 0)
    {
        _delay_ms(1);
        velocidad--;
    }
}

int main(void)
{
    DDRC &= ~((1 << PC3) | (1 << PC4) | (1 << PC5));   // entradas 
    PORTC |= (1 << PC3) | (1 << PC4) | (1 << PC5) ;   // pull-up interno 

    //salidas
    DDRD |= (1 << PD7);
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);

    uint16_t vel;

    while (1)
    {
        // leer switch y elegir velocidad
        if (!(PINC & (1 << PC5)) && !(PINC & (1 << PC4)) &&!(PINC & (1 << PC3))) // ! busca 0
        {
            vel= pgm_read_word(&velocidades[0]);
        }
        else if (!(PINC & (1 << PC5)) && !(PINC & (1 << PC4)) && (PINC & (1 << PC3)))
        {
            vel= pgm_read_word(&velocidades[1]);
        }
        else if (!(PINC & (1 << PC5)) && (PINC & (1 << PC4)) && !(PINC & (1 << PC3)))
        {
            vel= pgm_read_word(&velocidades[2]);
        }
        else if (!(PINC & (1 << PC5)) && (PINC & (1 << PC4)) && (PINC & (1 << PC3)))
        {
            vel= pgm_read_word(&velocidades[3]);
        }
         else if ((PINC & (1 << PC5)) && !(PINC & (1 << PC4)) && !(PINC & (1 << PC3)))
        {
            vel= pgm_read_word(&velocidades[4]);
        }
         else if ((PINC & (1 << PC5)) && !(PINC & (1 << PC4)) && (PINC & (1 << PC3)))
        {
            vel= pgm_read_word(&velocidades[5]);
        } else if ((PINC & (1 << PC5)) && (PINC & (1 << PC4)) && !(PINC & (1 << PC3)))
        {
            vel= pgm_read_word(&velocidades[6]);
        } else if ((PINC & (1 << PC5)) && (PINC & (1 << PC4)) && (PINC & (1 << PC3)))
        {
            vel= pgm_read_word(&velocidades[7]);
        }
       // LED 1
        PORTD |= (1 << PD7);
        retardo_ms(vel);
        PORTD &= ~(1 << PD7);

        // LED 2
        PORTB |= (1 << PB0);
        retardo_ms(vel);
        PORTB &= ~(1 << PB0);

        // LED 3
        PORTB |= (1 << PB1);
        retardo_ms(vel);
        PORTB &= ~(1 << PB1);

        // LED 4
        PORTB |= (1 << PB2);
        retardo_ms(vel);
        PORTB &= ~(1 << PB2);

         // LED 3
        PORTB |= (1 << PB1);
        retardo_ms(vel);
        PORTB &= ~(1 << PB1);
        
        // LED 2
        PORTB |= (1 << PB0);
        retardo_ms(vel);
        PORTB &= ~(1 << PB0);
        
    }
    return 0;
}

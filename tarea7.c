#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{

    DDRD &= ~(0x7F);      
    PORTD |= 0x7F;        

    DDRD |= (1 << PD7);

    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);

    PORTD &= ~(1 << PD7);
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));


    while (1)
    {
        uint8_t switches = PIND;
        uint8_t nibble1 = switches & 0x0F;
        uint8_t nibble2 = (switches >> 4) & 0x0F;
        uint8_t resultado_and = nibble1 & nibble2;

        if (resultado_and & (1 << 0))
            PORTD |= (1 << PD7);
        else
            PORTD &= ~(1 << PD7);
        if (resultado_and & (1 << 1))
            PORTB |= (1 << PB0);
        else
            PORTB &= ~(1 << PB0);
        if (resultado_and & (1 << 2))
            PORTB |= (1 << PB1);
        else
            PORTB &= ~(1 << PB1);
        if (resultado_and & (1 << 3))
            PORTB |= (1 << PB2);
        else
            PORTB &= ~(1 << PB2);

        _delay_ms(500);
        uint8_t resultado_or = nibble1 | nibble2;

        if (resultado_or & (1 << 0))
            PORTD |= (1 << PD7);
        else
            PORTD &= ~(1 << PD7);

        if (resultado_or & (1 << 1))
            PORTB |= (1 << PB0);
        else
            PORTB &= ~(1 << PB0);

        if (resultado_or & (1 << 2))
            PORTB |= (1 << PB1);
        else
            PORTB &= ~(1 << PB1);

        if (resultado_or & (1 << 3))
            PORTB |= (1 << PB2);
        else
            PORTB &= ~(1 << PB2);
        _delay_ms(500);
        uint8_t resultado_xor = nibble1 ^ nibble2;

        if (resultado_xor & (1 << 0))
            PORTD |= (1 << PD7);
        else
            PORTD &= ~(1 << PD7);

        if (resultado_xor & (1 << 1))
            PORTB |= (1 << PB0);
        else
            PORTB &= ~(1 << PB0);

        if (resultado_xor & (1 << 2))
            PORTB |= (1 << PB1);
        else
            PORTB &= ~(1 << PB1);

        if (resultado_xor & (1 << 3))
            PORTB |= (1 << PB2);
        else
            PORTB &= ~(1 << PB2);

        _delay_ms(500);
    }

    return 0;
}

#include <avr/io.h>

.equ LED_PINA, 0
.equ LED_PINB, 1
.equ LED_PINC, 2

.equ DIP0, 0
.equ DIP1, 1

.section .text
.global main

main:
    ; ----- Configurar PB0, PB1 y PB2 como salida -----
    sbi _SFR_IO_ADDR(DDRB), LED_PINA
    sbi _SFR_IO_ADDR(DDRB), LED_PINB
    sbi _SFR_IO_ADDR(DDRB), LED_PINC

    ; ----- Configurar PC0 y PC1 como entrada -----
    cbi _SFR_IO_ADDR(DDRC), DIP0
    cbi _SFR_IO_ADDR(DDRC), DIP1

    ; ----- Activar pull-up interno -----
    sbi _SFR_IO_ADDR(PORTC), DIP0
    sbi _SFR_IO_ADDR(PORTC), DIP1


MAIN_LOOP:

    ; ----- LED A -----
    sbi _SFR_IO_ADDR(PORTB), LED_PINA
    rcall DELAY
    cbi _SFR_IO_ADDR(PORTB), LED_PINA
    rcall DELAY

    ; ----- LED B -----
    sbi _SFR_IO_ADDR(PORTB), LED_PINB
    rcall DELAY
    cbi _SFR_IO_ADDR(PORTB), LED_PINB
    rcall DELAY

    ; ----- LED C -----
    sbi _SFR_IO_ADDR(PORTB), LED_PINC
    rcall DELAY
    cbi _SFR_IO_ADDR(PORTB), LED_PINC
    rcall DELAY

    ; ----- LED B -----
    sbi _SFR_IO_ADDR(PORTB), LED_PINB
    rcall DELAY
    cbi _SFR_IO_ADDR(PORTB), LED_PINB
    rcall DELAY

    rjmp MAIN_LOOP


DELAY:

    ; ----- Leer puerto C -----
    in R21, _SFR_IO_ADDR(PINC)

    ; ----- Conservar solamente PC0 y PC1 -----
    andi R21, 0x03

    ; ----- PC1 PC0 = 11 -----
    cpi R21, 0x03
    breq VELOCIDAD1

    ; ----- PC1 PC0 = 10 -----
    cpi R21, 0x02
    breq VELOCIDAD2

    ; ----- PC1 PC0 = 01 -----
    cpi R21, 0x01
    breq VELOCIDAD3

    ; ----- PC1 PC0 = 00 -----
    rjmp VELOCIDAD4


; VELOCIDADES

VELOCIDAD1:
    ldi R18, 0x40
    rjmp RETARDO


VELOCIDAD2:
    ldi R18, 0x30
    rjmp RETARDO


VELOCIDAD3:
    ldi R18, 0x20
    rjmp RETARDO


VELOCIDAD4:
    ldi R18, 0x10
    rjmp RETARDO


; RETARDO

RETARDO:
    ldi R19, 0xFF

DELAY2:
    ldi R20, 0xFF

DELAY3:
    subi R20, 1
    brne DELAY3

    subi R19, 1
    brne DELAY2

    subi R18, 1
    brne RETARDO

    ret
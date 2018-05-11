/*
 * File:   main.c
 * Author: tanuki
 *
 * Created on October 22, 2017, 2:38 PM
 */

// PIC16F628A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define	_XTAL_FREQ	4000000

#include <xc.h>

int count = 0;
int status = 0;
int sw = 0;

void set_pin(int status)
{
    switch(status) {
      case 0:
        PORTA = 0x01;  // RA0
        break;
      case 1:
        PORTA = 0x05;  // RA0|RA2
        break;
      case 2:
        PORTA = 0x04;  // RA2
        break;
      case 3:
        PORTA = 0x06;  // RA2|RA1
        break;
      case 4:
        PORTA = 0x02;  // RA1
        break;
      case 5:
        PORTA = 0x0A;  // RA1|RA3
        break;
      case 6:
        PORTA = 0x08;  // RA3
        break;
      case 7:
        PORTA = 0x09;  // RA3|RA0
        break;
    }
/*
    switch(status) {
      case 0:
        RB0 = 1;
        PORTA = 0x01;  // RA0
        break;
      case 1:
        PORTA = 0x04;  // RA2
        break;
      case 2:
        PORTA = 0x02;  // RA1
        break;
      case 3:
        PORTA = 0x08;  // RA3
        break;
    }
*/
}

// 割り込み処理
void interrupt isr(void) {
    if(INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;
//        TMR0 = 32;
        ++count;
        if(count > 500) {
            RB0 ^= 1;
            count = 0;
        }
        if(sw) {
            if(RB4 && RB5) {
                sw = 0;
                status = 0;
            }
        }
        else {
            if(!RB4 || !RB5) {
                sw = 1;
            }
        }
        if(!sw) {
            PORTA = 0;
            return;
        }

        if(RB4) ++status;
        if(RB5) --status;
        status %= 8;
//        status %= 4;
        set_pin(status);
    }
}

void initialize() {
    CMCON = 0x07;
    PORTA = 0;
    PORTB = 0;
    TRISA = 0;
//    TRISB = 0x32;  // RB1 => RX |   RB4 | RB5
    TRISB = 0x30;  // RB4 | RB5

    // タイマ0設定
    OPTION_REG = 0x02;  // Pre = 1/4
    TMR0 = 0;

    INTCON = 0xE0;  // GIE | PEIE | T01E

    RB0 = 0;
}

void main(void) {
    int len;
    char buf[16];

    initialize();
    set_pin(status);
    while(1) {
        __delay_ms(50);
    }
    return;
}


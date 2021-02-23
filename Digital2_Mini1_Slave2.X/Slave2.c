/*
 * File:   Slave2.c
 * Author: santi
 *
 * Created on February 22, 2021, 3:54 PM
 */


#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include "SPI.h"
//******************************************************************************
//Configuration Bits
//******************************************************************************
// CONFIG1
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//******************************************************************************
//Variables
//******************************************************************************
#define _XTAL_FREQ 8000000
uint8_t contador;
uint8_t OK;
//******************************************************************************
//Prototipos de Funciones
//******************************************************************************
void setup(void);

//******************************************************************************
//Loop Principal
//******************************************************************************

void main(void) {
    setup();
    while (1) {
       spiWrite(PORTD);
    }
}



//******************************************************************************
//Setup
//******************************************************************************

void setup(void) {
    ANSEL = 0; //Canales digitales
    ANSELH = 0;
    TRISD = 0;
    TRISC = 0;
    TRISA = 0;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    TRISAbits.TRISA5 = 1;
    PORTD = 0;
    PORTC = 0;
    INTCONbits.GIE = 1; //Activamos las interrupciones de todo
    INTCONbits.PEIE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    IOCBbits.IOCB0 = 1; //Asignamos el interrupt-on-change a los bits RB0 y RB1
    IOCBbits.IOCB1 = 1;
    contador = 0;
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

void __interrupt() oli(void) {
    if (INTCONbits.RBIF == 1) { //Interrupcion del interrupt-on-change para los botones
        if (PORTBbits.RB0 == 1) {
            PORTD++; //aumento en 1 el puerto D, pues este es mi contador
        } else if (PORTBbits.RB1 == 1) {
            PORTD--; //disminuyo en 1 el puerto D, pues este es mi contador
        }
        __delay_ms(200); //hago un delay de 200ms como antirrebote
        INTCONbits.RBIF = 0;
    }else if (PIR1bits.SSPIF == 1) {
        OK = spiRead();
        spiWrite(PORTD);
        PIR1bits.SSPIF = 0;
    }
}

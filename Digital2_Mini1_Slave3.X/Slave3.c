/*
 * File:   Slave3.c
 * Author: santi
 *
 * Created on February 22, 2021, 6:07 PM
 */


#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include "ADC.h"
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
#define verde RD2
#define amarillo RD1
#define rojo RD0
uint8_t ADC_value; //Variable que toma el valor del ADC luego de la conversion
uint8_t ADC_finish; // Bandera que me sirve para saber si ya se hizo la conversion
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
    ADC_setup(3, 2, 0, 0); //Aqui llamo a mi funcion de la libreria del ADC
    ADCON0bits.GO = 1; //Empiezo a hacer la primera conversion antes de entrar al loop
    while (1) {
        if (ADC_finish == 1) { //Reviso bandera pa ver si ya puedo empezar la siguiente conversion
            ADC_finish = 0;
            __delay_us(400);
            ADCON0bits.GO = 1;
        }
        /*Se estudio el sensor de temperatura y se encontro que el valor digital 
         que marcaba luego de la conversion ADC al estar en una temperatura de 25 
         grados era de 13 en decimales, para 36 grados era de 18 en decimales*/
        if (ADC_value < 13) { //Para temperatura menor a 25 grados C
            verde = 1;
            amarillo = 0;
            rojo = 0;
        } else if ((ADC_value >= 13)&(ADC_value <= 18)) {//Para temperatura entre 25 y 36 grados C
            verde = 0;
            amarillo = 1;
            rojo = 0;
        } else { //Para temperatura mayor a 36 grados C
            verde = 0;
            amarillo = 0;
            rojo = 1;
        }
        spiWrite(ADC_value);
    }
}



//******************************************************************************
//Setup
//******************************************************************************

void setup(void) {
    ANSEL = 0; //Canales diitales
    ANSELH = 0;
    ANSELbits.ANS2 = 1; //Solo el bit RA2 como analogico
    TRISD = 0;
    TRISC = 0;
    TRISA = 0;
    TRISB = 0;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA5 = 1;
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC3 = 1;
    PORTD = 0;
    PORTC = 0;
    PORTB = 0;
    INTCONbits.GIE = 1; //Activamos las interrupciones de todo
    INTCONbits.PEIE = 1;
    ADC_finish = 0; //Iniciamos nuestras variables y banderas en 0
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

void __interrupt() oli(void) {
    if (PIR1bits.ADIF == 1) { //Interrupcion del ADC
        PIR1bits.ADIF = 0;
        ADC_value = ADRESH; //Se cargan los 8 MSB del resultado al registro
        ADC_finish = 1;
    }else if (PIR1bits.SSPIF == 1) {
        OK = spiRead();
        spiWrite(ADC_value);
        PIR1bits.SSPIF = 0;
    }

}
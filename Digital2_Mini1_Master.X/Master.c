/*
 * File:   Master.c
 * Author: santi
 *
 * Created on February 22, 2021, 7:43 PM
 */


#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f887.h>
#include "LCD8.h"
#include "EUSART.h"
#include "SPI.h"
//**********
//Configuration Bits
//**********
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

//**********
//Variables
//**********
#define _XTAL_FREQ 8000000
#define RS RB0
#define EN RB1
uint8_t a;
uint8_t comando;
uint8_t Sensor1, Sensor2, Sensor3;

//**********
//Prototipos de Funciones
//**********
void setup(void);
void WriteFloat(uint8_t fila, uint8_t columna, uint8_t valor);
//**********
//Loop Principal
//**********

void main(void) {
    setup();
    Lcd_Init();
   // EUSART_conf();
    while (1) {

        PORTAbits.RA3 = 1;
        PORTAbits.RA1 = 0; //Slave1 Select
        //Codigo para recibir datos del Slave 1
        __delay_ms(1);
        spiWrite(1);
        Sensor1 = spiRead();
        __delay_ms(1);

        PORTAbits.RA1 = 1; //Slave2 select 
        PORTAbits.RA2 = 0;
        //Codigo para recibir datos del Slave 2
        __delay_ms(1);
        spiWrite(2);
        Sensor2 = spiRead();
        __delay_ms(1);

        PORTAbits.RA2 = 1;
        PORTAbits.RA3 = 0;
        //Codigo para recibir datos del Slave 3
        __delay_ms(1);
        spiWrite(3);
        Sensor3 = spiRead();
        __delay_ms(1);

        Lcd_Clear();
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String(" S1:  S2:  S3:");
        WriteFloat(2,1,Sensor1);
        WriteFloat(2,6,Sensor2);
        WriteFloat(2,11,Sensor3);

    }
}


//***********
//Setup
//***********

void setup(void) {
    //Configurar puertos
    TRISC = 0;
    TRISCbits.TRISC4 = 1; //Pin de entrada del SPI
    TRISCbits.TRISC7 = 1; //Pin de entrada del USART
    TRISD = 0;
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0;
    TRISB = 0;
    Sensor1 = 0;
    Sensor2 = 0;
    Sensor3 = 0;
    // CONFIGURAR INTERRUPCIONES
    INTCONbits.GIE = 1; //Se activan interrupciones generales
    INTCONbits.PEIE = 1; //Se activan interrupciones del periferio
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

//**********
//Interrupciones
//**********

void __interrupt() oli(void) {

    if (PIR1bits.RCIF) {
        comando = Receive();
        __delay_us(300);
        PIR1bits.RCIF = 0;
    }
}

void WriteFloat(uint8_t fila, uint8_t columna, uint8_t valor) {
    uint16_t temp;
    uint8_t unidad, dec, cent;

    //Tomamos los valores de unidad, decimal, centenal por separado en variables
    temp = valor * 1.961;
    unidad = temp / 100;
    temp = temp - unidad * 100;
    dec = temp / 10;
    temp = temp - dec * 10;
    cent = temp;

    //Las trnsformamos a un valor en su equivalente de ASCII para poder desplegarse
    unidad = unidad + 48;
    dec = dec + 48;
    cent = cent + 48;

    //Las despliego en la LCD segun la posicion indicada incluyendo el punto decimal
    Lcd_Set_Cursor(fila, columna);
    Lcd_Write_Char(unidad);
    Lcd_Set_Cursor(fila, columna + 1);
    Lcd_Write_String(".");
    Lcd_Set_Cursor(fila, columna + 2);
    Lcd_Write_Char(dec);
    Lcd_Set_Cursor(fila, columna + 3);
    Lcd_Write_Char(cent);
}

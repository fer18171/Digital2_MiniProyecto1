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
uint8_t prueba1, prueba2, prueba3;

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
    EUSART_conf();
    while (1) {
        Lcd_Clear();
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String(" S1:  S2:  S3:");
        WriteFloat(2,1,prueba1);
        WriteFloat(2,6,prueba2);
        WriteFloat(2,11,prueba3);
    }
}


//***********
//Setup
//***********

void setup(void) {
    //Inicializar la LCD
    //Configurar puertos
    TRISC = 0;
    TRISCbits.TRISC7 = 1;
    TRISD = 0;
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0;
    TRISB = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    prueba1=25;
    prueba2=129;
    prueba3=255;
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

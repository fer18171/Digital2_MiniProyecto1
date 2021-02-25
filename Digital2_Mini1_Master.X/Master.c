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
#define RS RA4
#define EN RA5
uint8_t a;
uint8_t comando;
uint8_t Sensor1, Sensor2, Sensor3;

//**********
//Prototipos de Funciones
//**********
void setup(void);
void WriteNumber(uint8_t fila, uint8_t columna, uint8_t valor, uint8_t formato);
//**********
//Loop Principal
//**********

void main(void) {
    setup();
    Lcd_Init();
    EUSART_conf();
    Lcd_Clear();
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
        spiWrite(2);
        Sensor3 = spiRead();
        __delay_ms(1);
        PORTB = Sensor3;
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String(" S1:   S2:   S3:");

        SendString("S1: ");
        WriteNumber(2, 1, Sensor1, 1);
        SendString(" S2: ");
        WriteNumber(2, 8, Sensor2, 0);
        SendString(" S3: ");
        WriteNumber(2, 13, Sensor3, 3);
        
        __delay_ms(500);
        for (int i = 0 ; i < 27 ; i++) {
            SendChar(8);
        }


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

    //Inicio del SPI
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

//**********
//Interrupciones
//**********

void __interrupt() oli(void) {

    if (!PIR1bits.RCIF) {
        comando = Receive();
        __delay_us(300);
        PIR1bits.RCIF = 0;
    } else if (!PIR1bits.TXIF) {

    }
}

void WriteNumber(uint8_t fila, uint8_t columna, uint8_t valor, uint8_t formato) {
    /*El valor formato es para elegir si se quiere colocar valor de voltaje
     * del ADC de 0V a 5V, un valor decimal de 0 a 255 o un valor de temperatura
     segun los rangos del sensor lm35
     * 
     * Formato 0 = decimal de 0 a 255
     * Formato 1 = float de 0.0V a 5.0V
       Formato 3 = Temperatura de -55 a 150 grados C */

    uint16_t temp; //Temp para manipular el valor de entrada
    uint8_t unidad, dec, cent; //Para tomar por separado cada cifra
    uint8_t cor; //Para hacer corrimiento en Set_Cursor de la columna dependiendo del formato elegido

    if (formato == 3) { //Para Temperatura
        cor = 1;
        if (valor > 68) { /*Dado que el valor de 0 grados C esta representado en el 
                            * valor digital 68, se toma por separado si el valor es 
                            mayor a 68, menor a 68 o igual a 68. Se multiplica por un 
                            * factor de 0.803 ya que ese es el resultado de 100*2.05/255 
                            el cual es el resultado de lo que aumenta el valor de temperatura
                            por cada aumento del valor digital*/
            temp = 0.803 * (valor - 68);
        } else if (valor < 68) {
            temp = 0.803 * (69 - valor);
        } else {
            temp = 0;
        }
    }//Tomamos los valores de unidad, decimal, centenal por separado en variables
    else if (formato == 1) { //Para voltaje de 0V a 5V
        cor = 0;
        temp = valor * 1.961; /*Aqui simplemente se multiplica por 1.961 para que
                                *  el resultado sea un valor de 0 a 500*/
    } else { //Para decimal de 0 a 255
        cor = 0;
        temp = valor;
    }

    //Se realiza el siguiente procedimiento para ir tomando cada cifra por separado
    unidad = temp / 100;
    temp = temp - unidad * 100;
    dec = temp / 10;
    temp = temp - dec * 10;
    cent = temp;

    //Las transformamos a un valor en su equivalente de ASCII para poder desplegarse
    //Se toma en cuenta que el 0 en ASCII es equivalente al numero 48, y los demas
    // van en orden desde 0 hasta 9
    unidad = unidad + 48;
    dec = dec + 48;
    cent = cent + 48;

    //Las despliego en la LCD segun la posicion indicada incluyendo el punto decimal
    //donde sea necesario u otros detalles dependiendo del formato elegido
    if ((formato == 3)&&(valor < 68)) { //Se coloca el - si es un valor negativo de T
        Lcd_Set_Cursor(fila, columna);
        Lcd_Write_String("-");
        SendString("-");
    } else if ((formato == 3)&&(valor >= 68)) {
        Lcd_Set_Cursor(fila, columna); //Si es positivo, no se coloca nada
        Lcd_Write_String(" ");
        SendString(" ");
    }
    Lcd_Set_Cursor(fila, columna + cor);
    Lcd_Write_Char(unidad);
    SendChar(unidad);
    if ((formato == 1)) { //Solo para el formato 1 se coloca el punto decimal
        Lcd_Set_Cursor(fila, columna + 1 + cor);
        Lcd_Write_String(".");
        SendString(".");
    }
    Lcd_Set_Cursor(fila, columna + 1 * (formato != 0) + 1);
    Lcd_Write_Char(dec);
    SendChar(dec);
    Lcd_Set_Cursor(fila, columna + 1 * (formato != 0) + 2);
    Lcd_Write_Char(cent);
    SendChar(cent);
    if (formato == 1) { //Solo para formato 1 se coloca V, porque es una lectura de voltaje
        Lcd_Set_Cursor(fila, columna + 1 * (formato != 0) + 3);
        Lcd_Write_String("V");
        SendString("V");
    }

}


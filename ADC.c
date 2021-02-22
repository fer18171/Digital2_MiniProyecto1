#include "ADC.h"

void ADC_setup(uint8_t ConClock, uint8_t Channel, uint8_t Format, uint8_t Vref) {
    switch (ConClock) { //Switch para elegir el conversion clock del ADC
        case 1:
            ADCON0bits.ADCS = 0b00; //  Fosc/2
            break;
        case 2:
            ADCON0bits.ADCS = 0b01; 
            break;
        case 3:
            ADCON0bits.ADCS = 0b10;
            break;
        case 4:
            ADCON0bits.ADCS = 0b11;
            break;
        default:
            ADCON0bits.ADCS = 0b10;
            break;
    }
    switch (Channel) { //Eleccion del canal
        case 0:
            ADCON0bits.CHS = 0b0000;
            break;
        case 1:
            ADCON0bits.CHS = 0b0001;
            break;
        case 2:
            ADCON0bits.CHS = 0b0010;
            break;
        case 3:
            ADCON0bits.CHS = 0b0011;
            break;
        case 4:
            ADCON0bits.CHS = 0b0100;
            break;
        case 5:
            ADCON0bits.CHS = 0b0101;
            break;
        case 6:
            ADCON0bits.CHS = 0b0110;
            break;
        case 7:
            ADCON0bits.CHS = 0b0111;
            break;
        case 8:
            ADCON0bits.CHS = 0b1000;
            break;
        case 9:
            ADCON0bits.CHS = 0b1001;
            break;
        case 10:
            ADCON0bits.CHS = 0b1010;
            break;
        case 11:
            ADCON0bits.CHS = 0b1011;
            break;
        case 12:
            ADCON0bits.CHS = 0b1100;
            break;
        case 13:
            ADCON0bits.CHS = 0b1101;
            break;
        case 14:
            ADCON0bits.CHS = 0b1110;
            break;
        case 15:
            ADCON0bits.CHS = 0b1111;
            break;
        default:
            ADCON0bits.CHS = 0b0000;
            break;
    }
    if (Format == 1) { //Eleccion del formato asi tipo a la izq o a la der
        ADCON1bits.ADFM = 1;
    } else {
        ADCON1bits.ADFM = 0;
    }
    switch (Vref) { //Eleccion de los voltajes de referencia
        case 1:
            ADCON1bits.VCFG0 = 0;
            ADCON1bits.VCFG1 = 0;
            break;
        case 2:
            ADCON1bits.VCFG0 = 1;
            ADCON1bits.VCFG1 = 1;
            break;
        case 3:
            ADCON1bits.VCFG0 = 0;
            ADCON1bits.VCFG1 = 1;
            break;
        case 4:
            ADCON1bits.VCFG0 = 1;
            ADCON1bits.VCFG1 = 0;
            break;
        default:
            ADCON1bits.VCFG0 = 0;
            ADCON1bits.VCFG1 = 0;
            break;
    }
    ADCON0bits.ADON=1; //Activacion de los enables de las interrupciones
    INTCONbits.GIE=1;
    INTCONbits.PEIE=1;
    PIE1bits.ADIE=1;
}

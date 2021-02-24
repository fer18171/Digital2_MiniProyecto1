#include "EUSART.h"

void EUSART_conf(void) {
    TXSTAbits.TXEN = 1;
    PIE1bits.RCIE = 0;
    PIE1bits.TXIE = 0;
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    TXSTAbits.TX9 = 0;
    RCSTAbits.RC9 = 0;
    BAUDCTLbits.BRG16 = 0;
    SPBRG = ((_XTAL_FREQ / 16) / 9600) - 1;
    TXSTAbits.BRGH = 1;
    SPBRGH = 0;
    RCSTAbits.CREN = 1;
}

void SendChar(char X) {
    while (!TXIF);
    TXREG = X;
}

void SendString(char* X){
    while (*X)
        SendChar(*X++);
}

uint8_t Receive(void) {
    if (RCSTAbits.OERR) {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }
    while (PIR1bits.RCIF == 0) {
    }
    return RCREG;
}

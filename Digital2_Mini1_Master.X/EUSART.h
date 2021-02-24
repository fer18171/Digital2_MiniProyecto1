
#ifndef EUSART_H
#define	EUSART_H

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

void EUSART_conf(void);
uint8_t Receive(void);
void SendChar(char X);
void SendString(char* X);

#endif	/* XC_HEADER_TEMPLATE_H */


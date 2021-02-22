/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
void ADC_setup(uint8_t ConClock, uint8_t Channel, uint8_t Format, uint8_t Vref);

#endif	/* ADC_H */


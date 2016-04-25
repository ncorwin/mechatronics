#include "spi.h"

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

// initialize spi1
void spi_init() {
    // Set RPB8 for SDO1
    RPB8Rbits.RPB8R = 0b0011;

    TRISBbits.TRISB7 = 0; //Set the CS pin to be a digital output
    CS = 1; // Start as high so nothing goes through

    // setup SPI Comms
    SPI1CON = 0;              // turn off the spi module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 11999;            // baud rate to 10 MHz [SPI1BRG = (40000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 1
}

void setVoltage(char channel, unsigned char voltage) {
    
    unsigned short buffer = 0x0;
    buffer = buffer | channel << 15;
    buffer = buffer | 0x7 << 12;
    buffer = buffer | voltage << 4;
    
    CS = 0;
    spi_io((buffer & 0xFF00) >> 8);
    spi_io(buffer & 0x00FF);
    LATAbits.LATA4 = !LATAbits.LATA4;
    CS = 1;
}
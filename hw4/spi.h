#ifndef SPI_H__
#define SPI_H__

#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

#define CS LATBbits.LATB7

unsigned char spi_io(unsigned char o); // send a byte via spi and return the response

void spi_init(void); // initialize spi1
void setVoltage(char channel, unsigned char voltage); // HW4

#endif
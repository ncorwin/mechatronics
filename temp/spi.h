#ifndef SPI_H__
#define SPI_H__

#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

unsigned char spi_io(unsigned char o); // send a byte via spi and return the response

void spi_init(void); // initialize spi1
void spi_write(unsigned short addr, const char data[], int len); // write len bytes to the ram, starting at the address addr
void spi_read(unsigned short addr, char data[], int len); // read len bytes from ram, starting at the address addr
void setVoltage(char channel, char voltage); // HW4

#endif
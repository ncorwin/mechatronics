#ifndef SPI_H__
#define SPI_H__

unsigned char spi_io(unsigned char ); // send a byte via spi and return the response

void spi_init(void); // initialize spi1
void spi_write(unsigned short, const char, int); // write len bytes to the ram, starting at the address addr
void spi_read(unsigned short, char, int); // read len bytes from ram, starting at the address addr
void setVoltage(char, char) // HW4

#endif
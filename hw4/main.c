#include "pic_config.h"
#include <math.h>

#define CS LATBbits.LATB15

#define NUMSAMPS 1000

unsigned char Sine[NUMSAMPS];
unsigned char Sawtooth[NUMSAMPS];

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

// initialize spi1
void spi_init() {
  // set up the chip select pin as an output
  // the chip select pin is used by the sram to indicate
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  TRISBbits.TRISB15 = 0;
  CS = 1;

  // Master - SPI1, pins are: SDI1(A1), SDO1(B13), SCK1(B14).
  RPB13Rbits.RPB13R = 0b0011; 
  
  // we manually control SS1 as a digital output (B15)
  // since the pic is just starting, we know that spi is off. We rely on defaults here
 
  // setup spi1
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 10000;            // baud rate to 10 MHz [SPI4BRG = (80000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1
}

void setVoltage(char channel, unsigned char voltage) {
    unsigned short buffer = 0x7000;
    
    buffer |= channel << 15;
    buffer |= voltage << 4;
    
    CS = 0; 
    spi_io((buffer & 0xFF00) >> 8);
    spi_io(buffer & 0x00FF);
    CS = 1;
}

void makeWaveform() {
    int i = 0;
    int center = 255/2, A = 255/2;
    for (i = 0; i < NUMSAMPS; i++) {
        Sine[i] = (unsigned char) (center + A * sin(2 * 3.14 * (i / NUMSAMPS)));
        Sawtooth[i] = (unsigned char) 255 * (i / NUMSAMPS);

    }
}

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; //Pin 12 is LED output
    TRISBbits.TRISB4 = 1; //Pin 11 is pushbutton input      
    LATAbits.LATA4 = 0;   //LED initilized off
         
    __builtin_enable_interrupts();
    
    spi_init();
    
    setVoltage(0, 0);
    
    makeWaveform();
    
    int i = 0;
    
    while(1) {
        
        if (_CP0_GET_COUNT() > 24000) {
            setVoltage(0, Sine[i]);
            setVoltage(1, Sawtooth[i]);
            
            i = i + 1;
            if (i > NUMSAMPS) {
                i = 0;
            }
            
            _CP0_SET_COUNT(0);
        }
    }
}

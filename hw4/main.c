#include "pic_config.h"
#include "i2c_master.h"
#include "spi.h"
#include <math.h>

#define CS LATBbits.LATB15

#define NUMSAMPS 1000

unsigned char Sine[NUMSAMPS];
unsigned char Sawtooth[NUMSAMPS];

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

#include "pic_config.h"
#include "i2c_master.h"
#include "spi.h"
#include <math.h>

#define CS LATBbits.LATB15

#define NUMSAMPS 1000

static volatile unsigned char Sine[NUMSAMPS];
static volatile unsigned char Sawtooth[NUMSAMPS];

void makeWaveform() {
    int i = 0;
    int center = 255/2, A = 255/2;
    for (i = 0; i < NUMSAMPS; i++) {
        Sine[i] = (unsigned char)center + A * sin( 2 * 3.14 * i / (1000));
        Sawtooth[i] = (unsigned char) 255 * i /1000;

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
    i2c_master_setup();
    init_exp();
    
    makeWaveform();
    
    unsigned char volts = 0;
    char output = 0;
    setVoltage(output, volts);
      
    int a = 0;
    int test = 0;
    
    while(1) {
        
        //LATAbits.LATA4 = !LATAbits.LATA4;
        
        if (get_exp(7) == 1) {
            //LATAbits.LATA4 = 1;
            set_exp(0, 1); 
        }
        else {
            //LATAbits.LATA4 = 0;
            set_exp(0, 0); 
        }
        

        
        if (_CP0_GET_COUNT() > 2400) {
            a++;
            //LATAbits.LATA4 = !LATAbits.LATA4;
            setVoltage(0, Sine[a]);
            setVoltage(1, Sawtooth[a]);
   
            _CP0_SET_COUNT(0);
        }
        if (a > NUMSAMPS-2) {
                a = 0;
        }
    }
}

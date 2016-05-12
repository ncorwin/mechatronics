#include "pic_config.h"
#include "i2c_master.h"
#include "ILI9163C.h"
#include <math.h>

int main() {

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
        
    SPI1_init();
    LCD_init();
    
    __builtin_enable_interrupts();

    
    LCD_clearScreen(WHITE);
    
    //LCD_drawCharacter(10,10,'d',BLUE);
    //LCD_drawPixel(1,1,RED);
    //LATAbits.LATA4 = 1;
    
    char c[100];
    int vari = 1337;
    sprintf(c,"Hello World %d!",vari);
    
    LCD_drawString(28,32,&c,BLUE);
    
    unsigned short testx, testy;
    
    testx = 10;
    testy = 10;
    
    while(1){
        
        //LCD_drawPixel(testx,testy,RED);
        //testx = testx + 1;
        //testy = testy + 1;
        
        if(PORTBbits.RB4 == 0) {
            LATAbits.LATA4 = !PORTAbits.RA4;
        }
    
    }
}



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
    //TRISASET = 0x10;
    TRISBbits.TRISB4 = 1; //Pin 11 is pushbutton input
    //TRISBSET = 0x10;
            
    LATAbits.LATA4 = 0;   //LED initilized off
    //LATASET = 0x10;
            
    __builtin_enable_interrupts();
    
    while(1) {
        if(PORTBbits.RB4 == 0) {
            LATAbits.LATA4 = PORTAbits.RA4;
        }
        else {            
            //LATAbits.LATA4 = 0;
            _CP0_SET_COUNT(0);
            LATAbits.LATA4 = !PORTAbits.RA4;
            while(_CP0_GET_COUNT() < 40000000 * 0.00028) {
                ;
            } 
        }

    }
    
    
}
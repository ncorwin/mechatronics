#include "pic_config.h"
#include "i2c_master.h"
#include "ILI9163C.h"
#include <math.h>

//Global Variables
signed short temp = 0, accel_x= 0, accel_y=0, accel_z=0, gyro_x=0, gyro_y=0, gyro_z=0;

//void __ISR(_TIMER_2_VECTOR, IPL5SOFT) PWM(void){
//    OC1RS = 3000*(float)(accel_x + (0xFFFF/2) + 1)/0XFFFF; // OC1RS = 1500;
//    OC2RS = 3000*(float)(accel_y + (0xFFFF/2) + 1)/0xFFFF; // OC2RS = 1500;
//    IFS0bits.T2IF = 0;
//}

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
    
    //nitOC();
    i2c_master_setup();
    init_imu();
    SPI1_init();
    LCD_init();
    
    __builtin_enable_interrupts();
    
    LCD_clearScreen(WHITE);
    
     _CP0_SET_COUNT(0);
    
    char voltage1=0, chann=0;
    float spi_scale = 122.0;
    unsigned char wave[1000], triangle[1000], val = 0x00;
    int level = 0, i = 0;
    char message[30];
    
    unsigned char tp[100], ax[100], ay[100], az[100], gx[100], gy[100], gz[100];
        
    while(1){
        
        if(PORTBbits.RB4 == 0) {
            LATAbits.LATA4 = !PORTAbits.RA4;
        }
        
        if (_CP0_GET_COUNT()>240000){
            //LATAbits.LATA4 = 1;
            val = getWho();
                if (val==0x69 ){
                     LATAbits.LATA4 = !LATAbits.LATA4;
                }
            
            _CP0_SET_COUNT(0);
            
            I2C_multiread(IMU, 0x20, data, 14); //may not even need to pass data here and can just save in the externed variable
            temp =(data[1]<< 8) | data[0];
            gyro_x =(data[3]<< 8) | data[2];
            gyro_y =(data[5]<< 8) | data[4];
            gyro_z =(data[7]<< 8) | data[6];
            accel_x =(data[9]<< 8) | data[8];
            accel_y =(data[11]<< 8) | data[10];
            accel_z =(data[13]<< 8) | data[12];
            
            //sprintf(tp,"temp: %1.2f", (float)(temp + (0xFFFF/2)+1)/0xFFFF);
            
            sprintf(ax,"Accel X: %1.2f", -2*9.81 + 9.81*4*(float)(accel_x + (0xFFFF/2)+1)/0xFFFF);
            sprintf(ay,"Accel Y: %1.2f", -2*9.81 + 9.81*4*(float)(accel_y + (0xFFFF/2)+1)/0xFFFF);
            sprintf(az,"Accel Z: %1.2f", -2*9.81 + 9.81*4*(float)(accel_z + (0xFFFF/2)+1)/0xFFFF);
            sprintf(gx,"Gyro X: %1.2f", -2*9.81 + 9.81*4*(float)(gyro_x + (0xFFFF/2)+1)/0xFFFF);
            sprintf(gy,"Gyro Y: %1.2f", -2*9.81 + 9.81*4*(float)(gyro_y + (0xFFFF/2)+1)/0xFFFF);
            sprintf(gz,"Gyro Z: %1.2f", -2*9.81 + 9.81*4*(float)(gyro_z + (0xFFFF/2)+1)/0xFFFF);
                        
        }
        
        //LCD_clearScreen(WHITE);
        
        //LCD_drawString(10,90,&tp,BLACK);
        
        LCD_drawString(10,10,&ax,RED);
        LCD_drawString(10,20,&ay,BLUE);
        LCD_drawString(10,30,&az,GREEN);
        LCD_drawString(10,50,&gx,RED);
        LCD_drawString(10,60,&gy,BLUE);
        LCD_drawString(10,70,&gz,GREEN);
    
    }
}


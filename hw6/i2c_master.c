#include "i2c_master.h"

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

unsigned char data[30];

void i2c_master_setup(void) {
  
  ANSELBbits.ANSB2 = 0;
  ANSELBbits.ANSB3 = 0;
  
  I2C2BRG = 90;// I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
  I2C2CONbits.ON = 1;               // turn on the I2C1 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { 
        Nop();
        Nop();
        Nop();    
     }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

void write_exp(unsigned char addr, unsigned char data){
   
    i2c_master_start();
    i2c_master_send(EXPANDER | 0);
    i2c_master_send(addr);
    i2c_master_send(data);
    i2c_master_stop();

}
unsigned char read_exp(unsigned char addr){
    unsigned char result;
    i2c_master_start();
    i2c_master_send(EXPANDER | 0);
    i2c_master_send(addr);
    i2c_master_restart();
    i2c_master_send(EXPANDER | 1);
    result = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return result;
}

void init_exp(void){
    //write_exp(0x05,0x38); //IOCON
    write_exp(0x00,0xF0); //IODIR
    write_exp(0x0A,0x00); //OLAT
    
}
void set_exp(int pin, int lvl){
    unsigned char out = 0x01;
    unsigned char test = read_exp(0x09); //GPIO
    
    out = out << pin;
    
    if (lvl == 1) {
        write_exp(0x0A, test | out); 
    }
    else if (lvl == 0)  {
        write_exp(0x0A, test & (~out)); 
    }

}
unsigned char get_exp(int pin){
    unsigned char out = 0x01;
    unsigned char test = read_exp(0x09); 
    
    out = out << pin;
    
    return (out & test) >> pin;
}

void write_imu(unsigned char addr, unsigned char data){
   
    i2c_master_start();
    i2c_master_send(IMU<<1 | 0);
    i2c_master_send(addr);
    i2c_master_send(data);
    i2c_master_stop();

}

void I2C_multiread(char add, char reg, unsigned char * data, char len){
    i2c_master_start(); // make the start bit
    i2c_master_send(add<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(reg); // the register to write to
    i2c_master_restart();
    i2c_master_send(add<<1 |1); // the value to put in the register
    int i;
    for(i=0;i<len-1;i++){
        data[i]= i2c_master_recv();
        i2c_master_ack(0);
    }
    data[len-1]=i2c_master_recv();
    i2c_master_ack(1); // make 
    i2c_master_stop();
}

void init_imu(void){
    write_imu(0x10,0x80); 
    write_imu(0x11,0x84); 
    
}

unsigned char getWho(){
    //char add = 0x20;//0b0100000;
    unsigned char r = 0x00;
    i2c_master_start(); // make the start bit
    i2c_master_send(0x6B<<1|0); // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(0x0F); // the register to read from
    i2c_master_restart(); // make the restart bit
    i2c_master_send(0x6B<<1|1); // write the address, shifted left by 1, or'ed with a 1 to indicate reading
    r = i2c_master_recv();
    i2c_master_ack(1); // make the ack so the slave knows we got it
    i2c_master_stop(); // make the stop bit
    return r;
    
}

#ifndef I2C_MASTER_NOINT_H__
#define I2C_MASTER_NOINT_H__

#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

#define EXPANDER 0x40
#define IMU 0b1101011
// Header file for i2c_master_noint.c
// helps implement use I2C1 as a master without using interrupts

void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz

void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop

void write_exp(unsigned char addr, unsigned char data);
unsigned char read_exp(unsigned char addr);
void init_exp(void);
void set_exp(int pin, int lvl);
unsigned char get_exp(int pin);

void write_imu(unsigned char addr, unsigned char data);
unsigned char read_imu(unsigned char addr);
void init_imu(void);
void set_imu(int pin, int lvl);
unsigned char get_imu(int pin);
void I2C_read_multiple(char address, char register, unsigned char * data, char length);
#endif
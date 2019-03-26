#include "mbed.h"
 
#define addr1 (0x6b<<1) //LSM9DS0のジャイロのスレーブアドレス
#define addr2 (0x1d<<1) //LSM9DS0の加速度・磁気のスレーブアドレス
 
I2C i2c(D14, D15);        // sda, scl
Serial pc(USBTX, USBRX);
 
char cmd[2];
 
void i2c_write(int addr,char regist,char data)
{   
    cmd[0]=regist;
    cmd[1]=data;
    i2c.write(addr,cmd,2);
}
 
char i2c_read(int addr,char regist)
{
    cmd[0]=regist;
    i2c.write(addr,cmd,1);
    i2c.read(addr,cmd,1);
    return cmd[0];
}
 
int i2c_read_16bit(int addr,char registL,char registH)
{
    return (int(i2c_read(addr,registH))<<8)|int(i2c_read(addr,registL));
}
 
int main() {
    i2c_write((0x68<<1),0x6b,0x00);
    i2c_write((0x68<<1),0x37,0x02);
    
    while(1){
    i2c_write((0x68<<1),0x6b,0x00);
    i2c_write((0x68<<1),0x37,0x02);
    pc.printf("AX:%d\n",i2c_read_16bit((0x68<<1),0x3c,0x3b));
    pc.printf(" ");
    pc.printf("AY:%d\n",i2c_read_16bit((0x68<<1),0x3e,0x3d));
    pc.printf(" ");
    pc.printf("AZ:%d\n",i2c_read_16bit((0x68<<1),0x40,0x3f));
    pc.printf("\n");
    wait(1);
    }
} 

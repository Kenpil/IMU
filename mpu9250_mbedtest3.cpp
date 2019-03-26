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

float u2s(int unsigneddata){
    if(unsigneddata & (0x01 << 15)){
        return -1 * ((unsigneddata ^ 0xffff) + 1);
    }else{
        return unsigneddata;
    }    
}        

int i2c_read_16bit(int addr,char registL,char registH)
{
    return (int(i2c_read(addr,registH))<<8)|int(i2c_read(addr,registL));
}
 
int main() {
    float ax = 0; float ay = 0; float az = 0; float gx = 0; float gy = 0; float gz = 0; float mx = 0; float my = 1; float mz = 1;
    i2c_write((0x68<<1),0x6b,0x00);
    i2c_write((0x68<<1),0x37,0x02);
    i2c_write((0x0c<<1),0x0a,0x12);

    while(1){
    //i2c_write((0x68<<1),0x6b,0x00);
    //i2c_write((0x68<<1),0x37,0x02);-1 * ((unsigneddata ^ 0xffff) + 1)
    ax = (-2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x68<<1),0x3c,0x3b));
    ay = (-2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x68<<1),0x3e,0x3d));
    az = (2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x68<<1),0x40,0x3f));
    gx = (2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x68<<1),0x44,0x43));
    gy = (2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x68<<1),0x46,0x45));
    gz = (2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x68<<1),0x48,0x47));
    mx = u2s(i2c_read_16bit((0x0c<<1),0x03,0x04)) / 32768.0f * 4800.0f;//(2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x0c<<1),0x03,0x04));
    my = u2s(i2c_read_16bit((0x0c<<1),0x05,0x06)) / 32768.0f * 4800.0f;//(2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x0c<<1),0x05,0x06));
    mz = u2s(i2c_read_16bit((0x0c<<1),0x07,0x08)) / 32768.0f * 4800.0f;//(2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x0c<<1),0x07,0x08));
    i2c_read((0x0c<<1), 0x09); 
    
    pc.printf("AX=%f", ax);
    pc.printf(" ");
    pc.printf("AY=%f", ay);
    pc.printf(" ");
    pc.printf("AZ=%f", az);
    pc.printf(" ");
    pc.printf("GX:%f", gx);
    pc.printf(" ");
    pc.printf("GY=%f", gy);
    pc.printf(" ");
    pc.printf("GZ=%f", gz);
    pc.printf(" ");
    pc.printf("MX=%f", mx);
    pc.printf(" ");
    pc.printf("MY=%f", my);
    pc.printf(" ");
    pc.printf("MZ=%f", mz);
    pc.printf("\n");
    
    wait(1);
    }
} 

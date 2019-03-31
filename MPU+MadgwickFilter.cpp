#include "mbed.h"
#include "MadgwickFilter.hpp"
#define addr1 (0x6b<<1) //LSM9DS0のジャイロのスレーブアドレス
#define addr2 (0x1d<<1) //LSM9DS0の加速度・磁気のスレーブアドレス
MadgwickFilter mf(0.0f);
float eulerval[3] = {0};
const float Pi = 3.1415;
float gyro_coefficient = (float) 500 / 32768;

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

float u2s(int unsigneddata)
{
    if(unsigneddata & (0x01 << 15)) {
        return -1 * ((unsigneddata ^ 0xffff) + 1);
    } else {
        return unsigneddata;
    }
}

float offset_gyro(int i)
{
    // extern float u2s(int unsigneddata); extern int i2c_read_16bit(int addr,char registL,char registH);
    float sum = 0.0f;
    for (int cnt = 0; cnt < 1000; cnt ++) {
        switch(i) {
            case 0:
                sum +=  gyro_coefficient * u2s(i2c_read_16bit((0x68<<1),0x44,0x43));
                break;// 360 * 2 * Pi;
            case 1:
                sum +=  gyro_coefficient * u2s(i2c_read_16bit((0x68<<1),0x46,0x45));
                break;// 360 * 2 * Pi;
            case 2:
                sum +=  gyro_coefficient * u2s(i2c_read_16bit((0x68<<1),0x48,0x47));
                break;// 360 * 2 * Pi;
        }
    }
    float offset = sum / 1000.0f;
    //pc.printf("offset(%d", i);
    //pc.printf(")=%f",offset);
    return offset;
}

int main()
{
    wait(1);
    int cnt = 0;
    float sumx = 0;
    float sumy = 0;
    float sumz = 0;
    float ax = 0;
    float ay = 0;
    float az = 0;
    float gx = 0;
    float gy = 0;
    float gz = 0;
    float mx = 0;
    float my = 1;
    float mz = 1;
    i2c_write((0x68<<1),0x6b,0x00);
    i2c_write((0x68<<1),0x37,0x02);
    i2c_write((0x0c<<1),0x0a,0x12);
    //Wire.write(0x1b);  //  0x00:2g, 0x08:4g, 0x10:8g, 0x18:16g
    i2c_write((0x68<<1),0x1b,0x08); //  0x00:250, 0x08:500, 0x10:1000, 0x18:2000deg/s
    float offset_gyro_x = offset_gyro(0);
    float offset_gyro_y = offset_gyro(1);
    float offset_gyro_z = offset_gyro(2);

    while(1){
    //for(int i = 0; i < 1000; i++) {
        ax = (-2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x68<<1),0x3c,0x3b));
        ay = (-2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x68<<1),0x3e,0x3d));
        az = (2.0f / float(0x8000)) * u2s(i2c_read_16bit((0x68<<1),0x40,0x3f));
        gx = (gyro_coefficient * u2s(i2c_read_16bit((0x68<<1),0x44,0x43)) - offset_gyro_x) / 360 * 2 * Pi;//(cof * u2s - offset)
        gy = (gyro_coefficient * u2s(i2c_read_16bit((0x68<<1),0x46,0x45)) - offset_gyro_y) / 360 * 2 * Pi;
        gz = (gyro_coefficient * u2s(i2c_read_16bit((0x68<<1),0x48,0x47)) - offset_gyro_z) / 360 * 2 * Pi;
        mx = (u2s(i2c_read_16bit((0x0c<<1),0x03,0x04)) / 32768.0f * 4921.0f * 10.0f - 48.0f) * 1.54f;//-48
        my = (u2s(i2c_read_16bit((0x0c<<1),0x05,0x06)) / 32768.0f * 4921.0f * 10.0f + 22.0f) * 1.54f;//+22
        mz = (u2s(i2c_read_16bit((0x0c<<1),0x07,0x08)) / 32768.0f * 4921.0f * 10.0f + 168.0f) * 1.54f;//+168
        //magX = mx / 32768.0f * 4800.0f;//[uT]に変換
        //magX = (mx + 344.0f) / 32768.0f * 4921.0f * 10.0f;//[mGauss]に変換
        //i2c_read((0x0c<<1), 0x09);

        mf.MadgwickAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);
        mf.getEulerAngle(eulerval);

        if(cnt % 100 == 1){
            pc.printf("roll:%f", eulerval[0]);
            pc.printf("  pitch:%f", eulerval[1]);
            pc.printf("  yaw:%f\n", eulerval[2]);
        }
        cnt = cnt + 1;
        /*pc.printf("%f", mx);
        pc.printf(" ");
        pc.printf("%f", my);
        pc.printf(" ");
        pc.printf("%f", mz);
        pc.printf("\n");
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
        wait(1);*/
    }
}

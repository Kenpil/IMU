#include "ros/ros.h"
#include "std_msgs/Float32MultiArray.h"
#include "MadgwickFilter.hpp"

MadgwickFilter mf(0.5f);
int mfgetcnt = 0;
float eulerval[10] = {0.0f};
const float Pi = 3.1415;
float gyro_coefficient = (float) 500 / 32768;

void getEulerAngle(float *val);
void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float durationtime);

void chatterCallback(const std_msgs::Float32MultiArray& msg){
    int num=msg.data.size();
    //ROS_INFO("I susclibed [%i]", num);
   //for(int i=0;i<num;i++){
   //     ROS_INFO("[%i]:%f", i, msg.data[i]);
   // }
   // for(int i = 0; i < num; i++){
        //msg.data[3], msg.data[4], msg.data[5]
        //msg.data[0], msg.data[1], msg.data[2]
        // msg.data[6], msg.data[7], msg.data[8], msg.data[9]
    mf.MadgwickAHRSupdate(msg.data[3], msg.data[4], msg.data[5], msg.data[0], msg.data[1], msg.data[2],  msg.data[6], msg.data[7], msg.data[8],  msg.data[9]);
    mf.getEulerAngle(eulerval);
    if(mfgetcnt % 500 == 0){
        ROS_INFO("roll:%f  pitch:%f  yaw:%f  time:%f\n", eulerval[0], eulerval[1], eulerval[2], msg.data[9]);
    } 
     mfgetcnt++;
}

int main(int argc, char **argv){
    ros::init(argc, argv, "basic_array_listener");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("array", 1000, chatterCallback);
    ros::Rate loop_rate(256);
    ros::spin();
    return 0;
}

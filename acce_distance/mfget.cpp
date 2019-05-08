#include "ros/ros.h"
#include "std_msgs/Float32MultiArray.h"
#include "MadgwickFilter.hpp"
#include <geometry_msgs/Twist.h>

MadgwickFilter mf(0.031f);
int mfgetcnt = 0;
float eulerval[10] = {0.0f};
float mfvelocity[3] = {0.0f};
const float Pi = 3.1415;
float gyro_coefficient = (float)500 / 32768;
float mfget_durationt = (float)1 / 256;

void getEulerAngle(float *val);
void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float mfget_durationt);

void chatterCallback(const std_msgs::Float32MultiArray &msg)
{
    //int num = msg.data.size();
    //ROS_INFO("I susclibed [%i]", num);
    //for(int i=0;i<num;i++){
    //     ROS_INFO("[%i]:%f", i, msg.data[i]);
    // }
    // for(int i = 0; i < num; i++){
    //msg.data[3], msg.data[4], msg.data[5]
    //msg.data[0], msg.data[1], msg.data[2]
    // msg.data[6], msg.data[7], msg.data[8], msg.data[9]
    mf.MadgwickAHRSupdate(msg.data[3], msg.data[4], msg.data[5], msg.data[0], msg.data[1], msg.data[2], msg.data[6], msg.data[7], msg.data[8], msg.data[9]);
    mf.getEulerAngle(eulerval);
    mfvelocity[0] += msg.data[0] * mfget_durationt;
    mfvelocity[1] += msg.data[1] * mfget_durationt;
    mfvelocity[2] += msg.data[2] * mfget_durationt;
    if (mfgetcnt % 500 == 0)
    {
        ROS_INFO("roll:%f  pitch:%f  yaw:%f  time:%f\n", eulerval[0], eulerval[1], eulerval[2], msg.data[9]);
        ROS_INFO("vx:%f  vy:%f  vz:%f\n", mfvelocity[0], mfvelocity[1], mfvelocity[2]);
    }
     //msg.data[0] = eulerval[0];
     //msg.data[1] = eulerval[1];
     //msg.data[2] = eulerval[2];     
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "basic_array_listener");
    ros::NodeHandle n;
    //ros::NodeHandle m;
    ros::Subscriber sub = n.subscribe("array", 1000, chatterCallback);
    ros::Publisher pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1);   
    //std_msgs::Float32MultiArray mfeulerangle;

    ros::AsyncSpinner spinner(1);
    spinner.start();
    ros::Rate rate(256);
    
    while (ros::ok())
    {
        n.subscribe("array", 1000, chatterCallback);
        /*mfeulerangle.data.resize(3);
        mfeulerangle.data[0] = eulerval[0] + 5.0f;
        mfeulerangle.data[1] = eulerval[1] + 5.0f;
        mfeulerangle.data[2] = eulerval[2] + 5.0f;
        pub.publish(mfeulerangle);*/
        geometry_msgs::Twist twist;
        twist.linear.x = mfvelocity[0];
        twist.linear.y = mfvelocity[1];
        twist.linear.z = mfvelocity[2];
        twist.angular.x = eulerval[0];
        twist.angular.y = eulerval[1];
        twist.angular.z = eulerval[2];
        if (mfgetcnt % 500 == 0)
        {
            ROS_INFO("I published!");
        }
        pub.publish(twist);
        //ROS_INFO("I published twist!");
        mfgetcnt++;
        rate.sleep();
    }
    spinner.stop();
    return 0;
}

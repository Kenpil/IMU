#include "ros/ros.h"
#include "std_msgs/Float32MultiArray.h"
#include "MadgwickFilter.hpp"
#include <geometry_msgs/Twist.h>
#include <mutex>

MadgwickFilter mf(0.031f);
int mfgetcnt = 1;
float eulerval[10] = {0.0f};
float mfvelocity[3] = {0.0f};
const float Pi = 3.1415;
const float GA = 9.80665;
float gyro_coefficient = (float)500 / 32768;
float mfget_durationt = (float)1 / 256;
float rpy_offset[3] = {0.0f};
int rpy_offset_cnt[3] = {0};
std::mutex m;

void getEulerAngle(float *val);
void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float mfget_durationt);

void rpy_offset_Callback(const std_msgs::Float32MultiArray &msg)
{
    ROS_INFO("rpy_offset_cnt!! = %d\n", rpy_offset_cnt[0]);

    mf.MadgwickAHRSupdate(msg.data[3], msg.data[4], msg.data[5], msg.data[0], msg.data[1], msg.data[2], msg.data[6], msg.data[7], msg.data[8], msg.data[9]);
    mf.getEulerAngle(eulerval);

    if (rpy_offset_cnt[0] % 100 == 0)
    {
        ROS_INFO("rpy_offset_cnt = %d\n", rpy_offset_cnt[0]);
    }

    for (int i = 0; i < 3; i++)
    {
        rpy_offset[i] += eulerval[i];
        rpy_offset_cnt[i]++;
    }
}

void chatterCallback(const std_msgs::Float32MultiArray &msg)
{
    //int num = msg.data.size();
    //ROS_INFO("I susclibed [%i]", num);
    //for(int i=0;i<num;i++){
    //     ROS_INFO("[%i]:%f", i, msg.data[i]);
    // }
    mf.MadgwickAHRSupdate(msg.data[3], msg.data[4], msg.data[5], msg.data[0], msg.data[1], msg.data[2], msg.data[6], msg.data[7], msg.data[8], msg.data[9]);
    mf.getEulerAngle(eulerval);
    for (int i = 0; i < 3; i++)
    {
        eulerval[i] -= rpy_offset[i];
        //mfvelocity[i] += msg.data[i] * msg.data[9];
    }
    mfvelocity[0] += GA * sin(eulerval[1]);
    mfvelocity[1] -= GA * sin(eulerval[0]);
    mfvelocity[2] -= GA * cos(eulerval[0]) * cos(eulerval[1]);

    if (mfgetcnt % 500 == 0)
    {
        ROS_INFO("roll:%f  pitch:%f  yaw:%f  time:%f\n", eulerval[0], eulerval[1], eulerval[2], msg.data[9]);
        ROS_INFO("vx:%f  vy:%f  vz:%f\n", mfvelocity[0], mfvelocity[1], mfvelocity[2]);
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "basic_array_listener");
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
    //ros::Subscriber sub = n.subscribe("array", 1000, chatterCallback);
    //ros::Subscriber sub2 = n.subscribe("array", 1000, rpy_offset_Callback);
    ros::Rate rate(256);
    ros::topic::waitForMessage<std_msgs::Float32MultiArray>("array", n);
    static bool IsFirst = true;

    ROS_INFO("start waiting");
    ros::Duration(3).sleep();
    ROS_INFO("start yaw_offset");
    ros::Subscriber sub2 = n.subscribe("array", 1000, rpy_offset_Callback);

    ros::AsyncSpinner spinner2(1);
    spinner2.start();
    while (IsFirst)
    {
        {
            std::lock_guard<std::mutex> lock(m);
            if (rpy_offset_cnt[0] % 100 == 0 && rpy_offset_cnt[0] > 0)
            {
                sub2.shutdown();
                ROS_INFO("shutdown_sub2");
                IsFirst = false;
            }
        }
        rate.sleep();
    }
    spinner2.stop();

    for (int i = 0; i < 3; i++)
    {
        rpy_offset[i] /= rpy_offset_cnt[i];
        ROS_INFO("rpy_offset=%f\n", rpy_offset[i]);
    }

    ros::Subscriber sub = n.subscribe("array", 1000, chatterCallback);
    ros::AsyncSpinner spinner(1);
    spinner.start();
    while (ros::ok())
    {
        ros::Subscriber sub = n.subscribe("array", 1000, chatterCallback);
        //mfeulerangle.data.resize(3);
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
            mfgetcnt = 0;
        }
        pub.publish(twist);
        //ROS_INFO("I published twist!");
        mfgetcnt++;
        rate.sleep();
    }
    spinner.stop();
    return 0;
}

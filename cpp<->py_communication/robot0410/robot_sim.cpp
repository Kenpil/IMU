#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_broadcaster.h>

#include <string>
#include <math.h>

void robot_pose_publish(float *position, float *rotation){
  static tf::TransformBroadcaster br;
  tf::Transform transform;
  transform.setOrigin( tf::Vector3(position[0], position[1], 0.0) );
  tf::Quaternion q;
  q.setRPY(rotation[0], rotation[1], rotation[2]);
  transform.setRotation(q);
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", "base_link"));
}

float position[3] = {0};//lx,ly,rz
float rotation[3] = {0};
float velocity[3] = {0}; //vx,vy,wz
void robot_tick(geometry_msgs::Twist cmd_vel, float dt){
  velocity[0]=cmd_vel.linear.x;
  velocity[1]=cmd_vel.linear.y;
  velocity[2]=cmd_vel.angular.z;
  float tmp_rz=position[2]+velocity[2]*dt/2;

  //position[0]+=(cos(tmp_rz)*velocity[0]-sin(tmp_rz)*velocity[1])*dt;
  //position[1]+=(sin(tmp_rz)*velocity[0]+cos(tmp_rz)*velocity[1])*dt;
  //position[2]+=velocity[2]*dt;

  rotation[0] += cmd_vel.angular.x * dt / 2;
  rotation[1] += cmd_vel.angular.y * dt / 2;
  rotation[2] += cmd_vel.angular.z * dt / 2;
}

geometry_msgs::Twist twist_last;
void twist_callback(const geometry_msgs::Twist& twist_msg){
  twist_last=twist_msg;
}

float normal_rad(float value){
  if(value>0)return fmod(value,3.1415);
  else return -fmod(-value,3.1415);
}

int main(int argc, char **argv){
  float dt = 1 / 256;
  ros::init(argc, argv, "vis_robot_sim");
  ros::NodeHandle nh;

  ros::Subscriber twist_sub= nh.subscribe("/cmd_vel", 10, twist_callback); 

  ros::Rate loop_rate(256); 
  while (ros::ok()){
    robot_tick(twist_last, 0.05);
    robot_pose_publish(position, rotation);
    ros::spinOnce();
    loop_rate.sleep();
  } 
  return 0;
}
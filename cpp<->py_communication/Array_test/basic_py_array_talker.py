#!/usr/bin/env python
# coding: UTF-8
import rospy
from std_msgs.msg import String, Float32MultiArray
    
def talker():
    pub = rospy.Publisher('array', Float32MultiArray, queue_size=10)
    rospy.init_node('talker', anonymous=True)
    r = rospy.Rate(10)
    a = Float32MultiArray() 
    while not rospy.is_shutdown():
        #str = "hello world %s"%rospy.get_time()
        #rospy.loginfo(str)
        a.data = [1, 2.2, 3.33]
        pub.publish(a)
        r.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException: pass

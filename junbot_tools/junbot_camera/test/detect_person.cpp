#include "ros/ros.h"
#include <std_msgs/Int32.h>
#include <vision_msgs/Detection2DArray.h>
#include <vision_msgs/Detection2D.h>
#include <iostream>
#include <std_msgs/String.h>

vision_msgs::Detection2D msg;

ros::Publisher pub_detection;

void detectionCallback(const vision_msgs::Detection2D &bbox_msgs) {
    float percent = 0;
    msg = bbox_msgs;
    percent = (msg.bbox.size_x*msg.bbox.size_x)/(1280*720);
    if (percent > 0.5){
        std_msgs::String msg;
        std::stringstream ss;
        ss << "warning";
        msg.data = ss.str();
        pub_detection.publish(msg);
    }
}


int main(int argc, char **argv) {

    ros::init(argc, argv, "dectect_person");
    ros::NodeHandle n;
    ros::Subscriber sub_detection = n.subscribe("/detectnet/detections", 1000, detectionCallback);
    pub_detection = n.advertise<std_msgs::String>("/warning_person", 1000); 

    while (ros::ok()) 
    {
        
        ros::Rate loop_rate(10);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}

#include "ros/ros.h"
#include <std_msgs/Int32.h>
#include <vision_msgs/Detection2DArray.h>
#include <vision_msgs/Detection2D.h>
#include <iostream>
#include <sstream>
#include <string>
#include <std_msgs/String.h>

ros::Publisher pub_detection;

void detectionCallback(const vision_msgs::Detection2DArray &bbox_msgs) {
    for (int i = 0; i < bbox_msgs.detections.size(); i++)
    {
        float percent = 0;
        percent = (bbox_msgs.detections.at(i).bbox.size_x*bbox_msgs.detections.at(i).bbox.size_y)/(1280*720);
        if (percent > 0.5){
            std_msgs::String msg;
            std::stringstream ss;
            ss << "warning";
            msg.data = ss.str();
            pub_detection.publish(msg);
        }
    }
}

void sonarCallback(const std_msgs::String::ConstPtr &sonar_msgs) {
    std::string tmp = sonar_msgs->data.c_str();
    std::stringstream ss(tmp);
    std::string distance[8];
    for (int i = 0; i < 8; i++)
    {
        std::getline(ss, distance[i], ':');
    }

    ROS_INFO("distance: ");
    std::cout << distance[0];
}


int main(int argc, char **argv) {

    ros::init(argc, argv, "dectect_person");
    ros::NodeHandle n;
    ros::Subscriber sub_detection = n.subscribe("/detectnet/detections", 1000, detectionCallback);
    ros::Subscriber sub_sonar = n.subscribe("/sonar_data", 1000, sonarCallback);
    pub_detection = n.advertise<std_msgs::String>("/warning_person", 1000); 

    while (ros::ok()) 
    {
        
        ros::Rate loop_rate(10);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}


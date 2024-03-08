#include "ros/ros.h"
#include <std_msgs/Int32.h>
#include <vision_msgs/Detection2DArray.h>
#include <vision_msgs/Detection2D.h>
#include <iostream>
#include <sstream>
#include <string>
#include <std_msgs/String.h>
#include <move_base_msgs/MoveBaseActionGoal.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/server/simple_action_server.h>
#include <move_base_msgs/MoveBaseAction.h>

ros::Publisher pub_detection;
ros::Publisher cancel;
bool detect = 0, sonar = 0;

void detectionCallback(const vision_msgs::Detection2DArray &bbox_msgs) {
    for (int i = 0; i < bbox_msgs.detections.size(); i++)
    {
        float percent = 0;
        percent = (bbox_msgs.detections.at(i).bbox.size_x*bbox_msgs.detections.at(i).bbox.size_y)/(1280*720);
        if (percent > 0.5){
            detect = 1;
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
        int temp = stoi(distance[i]);
        if (temp < 30){
            std_msgs::String msg;
            std::stringstream ss1;
            ss1 << "warning";
            msg.data = ss1.str();
            pub_detection.publish(msg);
        }
    }
    if (stoi(distance[0]) < 30 || stoi(distance[2]) < 30)
    {
        sonar = 1;
    }
    
}


int main(int argc, char **argv) {

    ros::init(argc, argv, "dectect_person");
    ros::NodeHandle n;
    ros::Subscriber sub_detection = n.subscribe("/detectnet/detections", 1000, detectionCallback);
    ros::Subscriber sub_sonar = n.subscribe("/sonar_data", 1000, sonarCallback);
    pub_detection = n.advertise<std_msgs::String>("/warning_person", 1000); 
    cancel = n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);
    std_msgs::String msg;
    std::stringstream ss;

    while (ros::ok()) 
    {
        if (detect == 1 && sonar == 1){
            ss << "warning";
            msg.data = ss.str();
            pub_detection.publish(msg);
            actionlib_msgs::GoalID tempCancel;
            tempCancel.stamp = {};
            tempCancel.id = {};
            cancel.publish(tempCancel);
            detect = 0;
            sonar = 0;
        }
        else {
            ss << "no_object";
            msg.data = ss.str();
            pub_detection.publish(msg);
        }
        ros::Rate loop_rate(10);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}


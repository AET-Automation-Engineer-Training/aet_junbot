#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_datatypes.h>
#include <move_base_msgs/MoveBaseActionGoal.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/server/simple_action_server.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <nlohmann/json.hpp>
#include "geometry_msgs/Twist.h"
#include <std_msgs/Int32.h>
#include <iostream>
#include <std_msgs/String.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
ros::Publisher cancel;


void cancelMissionCallback(const std_msgs::String::ConstPtr &msg_cancel) {
    ROS_INFO("abc");
    actionlib_msgs::GoalID tempCancel;
    tempCancel.stamp = {};
    tempCancel.id = {};
    cancel.publish(tempCancel);
}


int main(int argc, char **argv) {

    ros::init(argc, argv, "cancel_mission");
    ros::NodeHandle n;
    ros::Subscriber sub_cancel = n.subscribe("/mission_cancel", 1000, cancelMissionCallback);
    cancel = n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);

    while (ros::ok()) 
    {
        ros::Rate loop_rate(10);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_datatypes.h>
#include <move_base_msgs/MoveBaseActionGoal.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/server/simple_action_server.h>
#include <nlohmann/json.hpp>
#include "geometry_msgs/Twist.h"
#include <std_msgs/Int32.h>
#include <iostream>
#include <std_msgs/String.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"

struct pose_ {
    int ID_mission;
    float x,y,w;
};

move_base_msgs::MoveBaseActionGoal tempGoal;
geometry_msgs::PoseWithCovariance amcl_pose;
std::vector<pose_> target;

void goalCallback(const move_base_msgs::MoveBaseActionGoal::ConstPtr &msg) {
    tempGoal.goal_id = msg->goal_id;
    tempGoal.goal = msg->goal;
}

void missionCallback(const std_msgs::String::ConstPtr &msg_mission) {
    target.resize(2);
    using json = nlohmann::json;
    std::string tmp = msg_mission->data.c_str();
    if(tmp.size() <= 2)
    {
        return;
    }

    json parsedJson = json::parse(tmp);
    target.at(0).ID_mission = parsedJson["id"];
    target.at(1).ID_mission = parsedJson["id"];
    target.at(1).x = parsedJson["target_x"];
    target.at(1).y = parsedJson["target_y"];
    target.at(1).w = parsedJson["target_w"];
    target.at(0).x = parsedJson["ref_x"];
    target.at(0).y = parsedJson["ref_y"];
    target.at(0).w = parsedJson["ref_w"];
}

int main(int argc, char **argv) {

    tempGoal.goal.target_pose.pose.position.x = FLT_MAX;
    tempGoal.goal.target_pose.pose.position.y = FLT_MAX;

    ros::init(argc, argv, "mission_control");

    ros::NodeHandle n;

    ros::Subscriber sub_goal = n.subscribe("/move_base/goal", 1000, goalCallback);
    ros::Subscriber sub_mission = n.subscribe("/robot_target_id", 1000, missionCallback);
    
    while (ros::ok()) 
    {
        ros::Rate loop_rate(10);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}

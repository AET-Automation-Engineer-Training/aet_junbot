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

struct pose_ {
    int ID_mission;
    float x,y,w;
    int loopTime;
};
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
ros::Publisher cancel;
move_base_msgs::MoveBaseGoal goal;
ros::Publisher pub;
bool status_mission = 0;
int id_current;
int countLoop = 0;
std::vector<pose_> target;
geometry_msgs::PoseWithCovariance amcl_pose;


void missionCallback(const std_msgs::String::ConstPtr &msg_mission) {
    target.resize(2);
    using json = nlohmann::json;
    std::string tmp = msg_mission->data.c_str();
    if(tmp.size() <= 2)
    {
        return;
    }

    json parsedJson = json::parse(tmp);
    target.at(0).ID_mission = parsedJson["id1"];
    target.at(1).ID_mission = parsedJson["id2"];
    target.at(0).x = parsedJson["target1_x"];
    target.at(0).y = parsedJson["target1_y"];
    target.at(0).w = parsedJson["target1_w"];
    target.at(1).x = parsedJson["target2_x"];
    target.at(1).y = parsedJson["target2_y"];
    target.at(1).w = parsedJson["target2_w"];
    target.at(0).loopTime = parsedJson["loopTime"];
    target.at(1).loopTime = parsedJson["loopTime"];
    goal.target_pose.pose.position.x = target.at(0).x;
    goal.target_pose.pose.position.y = target.at(0).y;
    goal.target_pose.pose.orientation.w= target.at(0).w;
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
    id_current = target.at(0).ID_mission;

    status_mission = 1;
}

void poseAMCLCallback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg_amcl)
{
    amcl_pose = msg_amcl->pose;
}


int main(int argc, char **argv) {

    // goal.target_pose.pose.position.x = FLT_MAX;
    // goal.target_pose.pose.position.y = FLT_MAX;

    ros::init(argc, argv, "mission_control");
    MoveBaseClient ac("move_base", true);
    while(!ac.waitForServer(ros::Duration(5.0))){}
    ros::NodeHandle n;
    ros::Subscriber sub_mission = n.subscribe("/robot_target_id", 1000, missionCallback);
    ros::Publisher pub_mission = n.advertise<std_msgs::String>("/goal_arrived", 1000); 
    cancel = n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);
    ros::Subscriber sub_amcl = n.subscribe("/amcl_pose", 1000, poseAMCLCallback);
    std::string flag = "move";
    while (ros::ok()) 
    {
        if (status_mission == 1)
        {
            ROS_INFO("flag: %s", flag.c_str());
            if (flag == "move"){
                ac.sendGoal(goal);
                ROS_INFO("Send goal");
                flag = "moving";
            }
            double distance;
            distance = sqrt(pow(amcl_pose.pose.position.x - goal.target_pose.pose.position.x, 2) + pow(amcl_pose.pose.position.y - goal.target_pose.pose.position.y, 2));
            ROS_INFO("distance: %f", distance);
            if (distance <= 0.35)
            {
                actionlib_msgs::GoalID tempCancel;
                tempCancel.stamp = {};
                tempCancel.id = {};
                cancel.publish(tempCancel);
                ROS_INFO("Cancel goal");
                flag = "done";
            }

            if(flag == "done"){
                if (id_current == 1)
                {
                    goal.target_pose.pose.position.x = target.at(1).x;
                    goal.target_pose.pose.position.y = target.at(1).y;
                    goal.target_pose.pose.orientation.w= target.at(1).w;
                    id_current = target.at(1).ID_mission;
                }
                else if (id_current == 2)
                {
                    countLoop++;
                    goal.target_pose.pose.position.x = target.at(0).x;
                    goal.target_pose.pose.position.y = target.at(0).y;
                    goal.target_pose.pose.orientation.w= target.at(0).w;
                    id_current = target.at(0).ID_mission;
                }
                flag = "move";
                ROS_INFO("countLoop: %d", countLoop);
                if (countLoop == target.at(0).loopTime)
                {
                    status_mission = 0;
                    std_msgs::String msg;
                    std::stringstream ss;
                    ss << "done";
                    msg.data = ss.str();
                    pub_mission.publish(msg);
                    countLoop = 0;
                }
            }
        }
        
        ros::Rate loop_rate(10);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}

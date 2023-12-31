//
// Created by lacie on 03/06/2023.
//

#include "yolov5_detection.h"
#include "postprocess.h"
#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ros/ros.h>
#include <sensor_msgs/Image.h>

using namespace cv;

std::string model_path = "";

YoLoObjectDetection det(model);

cv::Mat curr_depthMap;
ros::Publisher obj_pub;

static std::vector<uint32_t> colors = {0xFF3838, 0xFF9D97, 0xFF701F, 0xFFB21D, 0xCFD231, 0x48F90A,
                                       0x92CC17, 0x3DDB86, 0x1A9334, 0x00D4BB, 0x2C99A8, 0x00C2FF,
                                       0x344593, 0x6473FF, 0x0018EC, 0x8438FF, 0x520085, 0xCB38FF,
                                       0xFF95C8, 0xFF37C7};

float objectDistance(cv::Rect object_rect)
{
    float dis = MAX_INT;

    return dis;
}

void imageRightRectifiedCallback(const sensor_msgs::Image::ConstPtr& msg)
{
  ROS_INFO("Right Rectified image received from ZED - Size: %dx%d", msg->width, msg->height);

  // TODO: Convert sensor_msgs::Image to cv::Mat
  cv::Mat input_image = msg->data;

  std::vector<Detection> res;
  det.detectObject(input_image, res);

  int fps = 1.0/time_used.count();
    std::string string_fps = "FPS: " + std::to_string(fps);
    bool check_obs = 0;

    for(auto object:res) {

        cv::Rect r = get_rect(input_image, object.bbox);
        cv::Mat mask = det.get_mask(input_image, object);

        auto color = colors[(int) object.class_id % colors.size()];
        auto bgr = cv::Scalar(color & 0xFF, color >> 8 & 0xFF, color >> 16 & 0xFF);

        cv::rectangle(input_image, r, bgr, 2);

        float dis = objectDistance(r);

        // TODO: Check distance and publish
    }

    cv::imshow("result", input_image);
    cv::waitKey(1);
}

void depthCallback(const sensor_msgs::Image::ConstPtr& msg)
{
    // Get a pointer to the depth values casting the data
    // pointer to floating point
    float* depths = (float*)(&msg->data[0]);

    // Image coordinates of the center pixel
    int u = msg->width / 2;
    int v = msg->height / 2;

    // Linear index of the center pixel
    int centerIdx = u + msg->width * v;

    // Output the measure
    ROS_INFO("Center distance : %g m", depths[centerIdx]);

    // Convert depths to cv::Mat
    curr_depthMap = depths;
}

int main( int argc, char** argv )
{

    ros::init(argc, argv, "junbot_camera_detector");

    ros::NodeHandle n;

    // TODO: Recheck ZED Mini Image topic
    ros::Subscriber subLeftRectified = n.subscribe("image", 10, imageLeftRectifiedCallback);
    ros::Subscriber subDepth = n.subscribe("depth", 10, depthCallback);

    ros::spin();

    return 0;
}






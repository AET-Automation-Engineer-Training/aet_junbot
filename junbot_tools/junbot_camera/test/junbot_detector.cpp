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
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

#define MAX_INT 1000000

using namespace cv;

std::string model_path = "";

YoLoObjectDetection det(model_path);

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

void imageLeftRectifiedCallback(const sensor_msgs::Image::ConstPtr& msg)
{
    ROS_INFO("Left Rectified image received from T265 - Size: %dx%d", msg->width, msg->height);

    // TODO: Convert sensor_msgs::Image to cv::Mat
    cv_bridge::CvImagePtr cv_ptr;
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);

    cv::Mat input_image = cv_ptr->image;

    std::vector<Detection> res;
    det.detectObject(input_image, res);

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
    cv::Mat depthMat(msg->height, msg->width, CV_32F, depths);
    curr_depthMap = depthMat.clone();
}

int main( int argc, char** argv )
{

    ros::init(argc, argv, "junbot_camera_detector");

    ros::NodeHandle n;

    // TODO: Recheck ZED Mini Image topic
    ros::Subscriber subLeftRectified = n.subscribe("/camera/fisheye1/image_raw/rectified", 10, imageLeftRectifiedCallback);
    ros::Subscriber subDepth = n.subscribe("/camera/fisheye1/image_raw/depth", 10, depthCallback);

    ros::spin();

    return 0;
}






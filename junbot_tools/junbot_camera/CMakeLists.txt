cmake_minimum_required(VERSION 2.4)

project(junbot_camera)

set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -std=c++14")
set(CUDA_TOOLKIT_ROOT_DIR "/usr/local/cuda")

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 OLD)
	cmake_policy(SET CMP0004 OLD)
	cmake_policy(SET CMP0015 OLD)
endif(COMMAND cmake_policy)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    SET(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

SET(EXECUTABLE_OUTPUT_PATH ".")
SET(SPECIAL_OS_LIBS "")

find_package(catkin REQUIRED COMPONENTS
        roscpp
        rospy
        std_msgs
        nav_msgs
        cv_bridge
        image_transport
        vision_msgs
        tf
        sensor_msgs
        )

catkin_package(
        INCLUDE_DIRS include
        CATKIN_DEPENDS roscpp std_msgs nav_msgs
        #  DEPENDS system_lib
)

find_package(OpenCV 4.5 REQUIRED NO_MODULE PATHS /usr/local NO_DEFAULT_PATH)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(CUDA REQUIRED)

message(${OpenCV_INCLUDE_DIRS})
message(${OpenCV_LIBRARIES})

IF(NOT WIN32)
    SET(SPECIAL_OS_LIBS "pthread" "X11")
    add_definitions(-Wno-write-strings)
ENDIF()

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLUT_INCLUDE_DIR})
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${catkin_INCLUDE_DIRS})
include_directories(
        # ${CMAKE_CURRENT_SOURCE_DIR}/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/Model
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/junbot_camera
)

link_directories(${GLEW_LIBRARY_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})

# cuda
include_directories(/usr/local/cuda/include)
link_directories(/usr/local/cuda/lib64)

# tensorrt
include_directories(/usr/include/x86_64-linux-gnu/)
link_directories(/usr/lib/x86_64-linux-gnu/)

FILE(GLOB_RECURSE SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cu
        ${CMAKE_CURRENT_SOURCE_DIR}/yolov5_detection.cpp
        )
FILE(GLOB_RECURSE HDR_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/yolov5_detection.h
        )

cuda_add_library(yolov5-detector SHARED ${HDR_FILES} ${SRC_FILES})

add_definitions(-std=c++14 -g -O3 -D_MWAITXINTRIN_H_INCLUDED -Wno-deprecated-declarations)

SET(TRT_LIBS nvinfer)

target_link_libraries(yolov5-detector
                        ${TRT_LIBS}
                        ${SPECIAL_OS_LIBS}
                        ${OPENGL_LIBRARIES}
                        ${GLUT_LIBRARY}
                        ${GLEW_LIBRARIES}
                        ${OpenCV_LIBRARIES}
                        ${CUDA_CUDA_LIBRARY}
                        ${CUDA_CUDART_LIBRARY}
                        -lopencv_dnn
        )

add_executable(${PROJECT_NAME}_detector test/junbot_detector.cpp)
add_executable(${PROJECT_NAME}_mission_control test/mission_control.cpp)
add_executable(${PROJECT_NAME}_detect_person test/detect_person.cpp)
add_executable(${PROJECT_NAME}_cancel_mission test/cancel_mission.cpp)

target_link_libraries(${PROJECT_NAME}_detector
   ${realsense2_LIBRARY}
   ${Boost_LIBRARIES}
   ${catkin_LIBRARIES}
   ${OpenCV_LIBRARIES}
   yolov5-detector
   -lopencv_dnn
)
target_link_libraries(${PROJECT_NAME}_mission_control
   ${realsense2_LIBRARY}
   ${Boost_LIBRARIES}
   ${catkin_LIBRARIES}
   ${OpenCV_LIBRARIES}
   yolov5-detector
)
target_link_libraries(${PROJECT_NAME}_detect_person
   ${realsense2_LIBRARY}
   ${Boost_LIBRARIES}
   ${catkin_LIBRARIES}
   ${OpenCV_LIBRARIES}
   yolov5-detector
)

target_link_libraries(${PROJECT_NAME}_cancel_mission
   ${realsense2_LIBRARY}
   ${Boost_LIBRARIES}
   ${catkin_LIBRARIES}
   ${OpenCV_LIBRARIES}
   yolov5-detector
)

install(TARGETS ${PROJECT_NAME}_detector
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(TARGETS yolov5-detector
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(TARGETS ${PROJECT_NAME}_mission_control
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
  install(TARGETS ${PROJECT_NAME}_detect_person
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(TARGETS ${PROJECT_NAME}_cancel_mission
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

catkin_install_python(PROGRAMS scripts/set_cams_transforms.py
                      DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

# Install launch files
install(DIRECTORY launch/
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)

install(DIRECTORY config/
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/config
)





# ATE-Junbot

## Hardware

- Jetson Orin Nano
- RPLidar A2
- ZED Mini

## Feature 

- 4 wheel mecanum kinematic
- Build 2D map with lidar (2D Lidar + ZED Mini Odometry)
- Navigation in 2D map
- Obstacle advoidance
- Object detection

## Docker

```bash
cd docker
docker build -t aet_junbot .

# run docker
./start_docker.sh

# stop docker
./stop_docker.sh
```


















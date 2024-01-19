# ATE-Junbot

## Hardware

- Jetson Orin Nano
- RPLidar A2
- T265

## Feature 

- 4-wheel mecanum kinematic
- Build 2D map with lidar (2D Lidar + VIO from T265)
- Navigation in 2D map
- Obstacle avoidance
- Object detection


## Docker

- Download a cuDNN 8.6.0 installer deb-package from NVIDIA's download page (login required).

- Place the cuDNN installer deb-package in this directory and rename it to cudnn-local-repo-$(dpkg --print-architecture).deb.

- Download a TensorRT 8.5.1 installer deb-package from NVIDIA's download page (login required).

- Place the TensorRT installer deb-package in this directory and rename it to nv-tensorrt-local-repo-$(dpkg --print-architecture).deb.

- Build the Docker image with below command.

```bash
cd docker

docker buildx build \
    --load \
    --platform $(uname)/$(uname -m) \
    --build-arg UBUNTU_VERSION=20.04 \
    --build-arg CUDA_VERSION=11.8 \
    --build-arg CUDNN_VERSION=8.6.0 \
    --build-arg TENSORRT_VERSION=8.5.1 \
    --build-arg INSTALL_AS_DEV=1 \
    --tag ate_junbot \
    .

# run docker
./start_docker.sh

# stop docker
./stop_docker.sh
```

## Related project

- [Firmware](https://github.com/AET-Automation-Engineer-Training/aet_open_hw/tree/main)
- [GUI](https://github.com/AET-Automation-Engineer-Training/aet_junbot_gui)
- [Mobile App](https://github.com/lacie-life/junbot_app)



















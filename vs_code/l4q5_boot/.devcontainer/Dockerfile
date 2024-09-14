FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    cmake \
    ninja-build \
    libtool \
    pkg-config \
    libftdi-dev \
    libusb-1.0-0-dev \
    automake \
    libncurses-dev \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    gdb-multiarch \
    stlink-tools \
    openocd

# if using GDB on the host instead of the container
EXPOSE 50000
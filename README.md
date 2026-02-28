# Passive Acoustic Surveillance System

This project is being designed for TUBITAK 2209-A project. It's basically passive 
acoustic surveillance system primarily designed for drones and helicopers. It 
consists of three parts:

+ Custom DSP library
+ Embedded firmware driver
+ Ground station application

For this project, a custom DSP library was designed. So that in ground station, 
the all signal processing operations are done over this library. It's also
published at my GitHub as separate repo.

To get the repo, use this command:

```bash
$ git clone https://github.com/CanGulmez/Digital-Signal-Processing.git
```

Also I put the library and header file in the **./lib** directory.

Embedded firmware was made with PlatformIO library. So please make sure that you
have it. It's Python 3 package. If you don't have it, get it with this command:

```bash
$ python3 install platformio --break-system-packages
```

PlatformIO project inherits the required CMSIS/HAL libraries defaulty. But you need
to install FreeRTOS kernel externally and put the **./lib** directory as I did. For
all settings including **platformio.ini** and **FreeRTOSConfig.h**, you can follow 
my specifications.

Embedded firmware was written for STM32H750x microcontroller. 

To build the firmware, use this command:

```bash
$ make firmware
```

This command will compile the firmware codebase and then flash it into the embedded
board. So connect the embedded board into your PC firstly. Another important point
is that this command use the  **openocd** command with st-link configuration. If you
have the another port like JTAG, edit the **Makefile**. If you don't have **openocd**,
install it with this command:

```bash
$ sudo apt install openocd		# there is no need any st-link driver for GNU/Linux.
```

At ground station side, it is done on the Linux machine so that the all codebase is
writting with POSIX standards. To build and then run the ground station, use this 
command:

```bash
$ make station
```

The ground station is integrated with many libraries that you have to install by
hand. To install the all required libraries, use this command:

```bash
$ sudo apt install check \
	libgsl-dev \
	libadwaita-1-dev \
	libgtk-4-dev \
	libshumate-dev \
	libsqlite3-dev \
	pkg-config
```

To run the project in isolated environment, you need to build a Docker image with
this command:

```bash
$ docker build -t sonar .
```

This command will create a **sonar** image using the Dockerfile instructions. This
project is based GTK-4 so that you need to specify that which window server (X11
or Wayland) you will use from the Docker image. I'm gonna use the X11 server for my
image. Firstly, enable it for Docker with this command:

```bash
$ xhost +local:docker
$ xhost +SI:localuser:root
```

After that, create and run the docker container with this command:

```bash
$ docker run -it --rm -e DISPLAY=$DISPLAY -e GDK_BACKEND=x11 -v /tmp/.X11-unix:/tmp/.X11-unix sonar
```

After entered into the **sonar** container, again run the ground station.

The ground station has four sub-modules:

+ Microphone
+ AI Model
+ Navigation
+ GPS Map

In `Micophone` sub-module, there are three sections. At left side, there is the
channel configurations. The embedded firmware can be read from many channels. 
At center, there is the signal analysises. As I said, it is done over my custom DSP
library. Also there are cartesian and polar plots to visualize the sensor data.

In `AI Model` sub-module, there is a deep learning model written in Python 3 with
**Tensorflow** library. At UI, there is a model parameter selection panel. This 
model is being run as a **child** process. It's made by **fork()/exec()** routines 
as general. Because, model fitting is computationally heavy.

In `Navigation` sub-module, the IMU (Interial Measurement Unit) sensor data is
being visualized.

In `GPS Map` sub-module, there is also a shumate map that shows the GPS module 
outputs.


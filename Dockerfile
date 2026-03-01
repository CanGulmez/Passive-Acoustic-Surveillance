# Containize passive acoustic surveillance project

FROM ubuntu:24.04

RUN apt update && apt install -y \
	check \
	libgsl-dev \
	libadwaita-1-dev \
	libgtk-4-dev \
	libshumate-dev \
	libsqlite3-dev \
	pkg-config \
	build-essential \
	make \
	dbus-x11

WORKDIR /home/SONAR/

COPY . /home/SONAR/

CMD ["/bin/bash"]

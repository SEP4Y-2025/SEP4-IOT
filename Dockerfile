FROM gcc:latest

WORKDIR /usr/src/app

# Install dependencies
RUN apt-get update && apt-get install -y libmosquitto-dev libcurl4-openssl-dev git cmake build-essential

# Clone and install cJSON
RUN git clone https://github.com/DaveGamble/cJSON.git && \
    cd cJSON && mkdir build && cd build && \
    cmake .. && make && make install && \
    cd ../.. && rm -rf cJSON

# Move cJSON.h into a cjson/ subdirectory to support <cjson/cJSON.h>
# RUN mkdir -p /usr/local/include/cjson && \
#     mv /usr/local/include/cJSON.h /usr/local/include/cjson/cJSON.h

RUN mkdir -p /usr/local/include/cjson && \
    if [ -f /usr/local/include/cJSON.h ]; then \
        mv /usr/local/include/cJSON.h /usr/local/include/cjson/cJSON.h; \
    fi


RUN ldconfig

# Copy source code into container
COPY . .

# Build the project
RUN make

# Run the compiled program
CMD ["./iot"]


# FROM gcc:latest

# WORKDIR /usr/src/app

# # Install the Mosquitto client library (libmosquitto)
# RUN apt-get update && apt-get install -y libmosquitto-dev libcurl4-openssl-dev git cmake build-essential

# RUN git clone https://github.com/DaveGamble/cJSON.git && \
#     cd cJSON && mkdir build && cd build && \
#     cmake .. && make && make install && \
#     cd ../.. && rm -rf cJSON

# # Move cJSON.h into a cjson/ subdirectory 
# RUN mkdir -p /usr/local/include/cjson && \
#     mv /usr/local/include/cJSON.h /usr/local/include/cjson/cJSON.h

# RUN ldconfig
# # Copy the IoT (C) source code into the container
# COPY main.c .

# # Compile the C program linking with the mosquitto library
# RUN gcc -o iot main.c -lmosquitto -lcjson

# # Run the compiled IoT client
# CMD ["./iot"]


# # Use the latest GCC image for building the IoT 
# FROM gcc:latest

# # Set the working directory in the container
# WORKDIR /usr/src/app

# # Install necessary packages, including Mosquitto and other dependencies
# RUN apt-get update && \
#     apt-get install -y \
#     libmosquitto-dev \
#     gcc-avr \
#     avr-libc \
#     binutils-avr \
#     libjson-c-dev \
#     libssl-dev

# # Copy the IoT (C) source code and drivers into the container
# COPY main.c . 
# COPY lib/ ./lib/

# # Compile the C program for AVR target (ATmega2560) and link the Mosquitto library
# RUN gcc -o iot main.c lib/drivers/*.c -lmosquitto

# # If you need to compile for the host architecture (for debugging or testing), do it separately
# #RUN gcc -o iot main.c lib/drivers/*.c -lmosquitto -ljson-c

# # By default, use the AVR build, but you can specify the host build if needed
# CMD ["./iot"]




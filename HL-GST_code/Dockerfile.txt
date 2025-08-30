FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
   apt-get install -y \
   build-essential \
   cmake \
   git \
   libboost-all-dev && \
   rm -rf /var/lib/apt/lists/*

WORKDIR /app/HL-GST_code

COPY . .

# RUN mkdir build && \
#    cd build && \
#    cmake .. && \
#    make

CMD ["/bin/bash"]
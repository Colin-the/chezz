# Use a basic base image
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    libssl-dev \
    zlib1g-dev \
    libbz2-dev \
    libreadline-dev \
    libsqlite3-dev \
    wget \
    curl \
    llvm \
    libncurses5-dev \
    libncursesw5-dev \
    xz-utils \
    tk-dev \
    libffi-dev \
    liblzma-dev \
    git \
    ca-certificates

# Download and build Python 3.11.2
WORKDIR /usr/src
RUN wget https://www.python.org/ftp/python/3.11.2/Python-3.11.2.tgz && \
    tar xzf Python-3.11.2.tgz && \
    cd Python-3.11.2 && \
    ./configure --enable-optimizations --with-ensurepip=install && \
    make -j$(nproc) && \
    make altinstall

# Make python3.11 the default and install project dependcys 
RUN ln -s /usr/local/bin/python3.11 /usr/local/bin/python
RUN ln -s /usr/local/bin/pip3.11 /usr/local/bin/pip
RUN pip install numpy
RUN apt-get update && \
    apt-get install -y swig
RUN apt-get update && \
    apt-get install -y python3-numpy-dev

# Set working directory
WORKDIR /app

# Go into the C folder and run make to compile the C code
CMD [ "cd C" ]
CMD [ "make" ]
CMD [ "cd .." ]

#start the server 
CMD ["python3.11", "server.py", "8000"]

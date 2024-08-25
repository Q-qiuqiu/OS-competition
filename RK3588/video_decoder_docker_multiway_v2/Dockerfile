FROM ubuntu:22.04 AS builder

RUN sed -i 's@//ports.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list

RUN apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata && \
    ln -fs /usr/share/zoneinfo/Asia/Shanghai /etc/localtime && \
    dpkg-reconfigure --frontend noninteractive tzdata

RUN apt-get -y install \
    autoconf \
    automake \
    build-essential \
    cmake \
    git-core \
    libass-dev \
    libfreetype6-dev \
    libgnutls28-dev \
    libmp3lame-dev \
    libsdl2-dev \
    libtool \
    libva-dev \
    libvdpau-dev \
    libvorbis-dev \
    libxcb1-dev \
    libxcb-shm0-dev \
    libxcb-xfixes0-dev \
    meson \
    ninja-build \
    pkg-config \
    texinfo \
    wget \
    yasm \
    zlib1g-dev \
    libopencv-dev

RUN mkdir -p /root/dev && cd /root/dev && \
    git clone -b jellyfin-mpp --depth=1 https://github.com/nyanmisaka/mpp.git rkmpp && \
    cd rkmpp && \
    mkdir rkmpp_build && \
    cd rkmpp_build && \
    cmake \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=ON \
      -DBUILD_TEST=OFF \
      .. && \
    make -j $(nproc) && \
    make install
	
RUN mkdir -p /root/dev && cd /root/dev && \
    git clone -b jellyfin-rga --depth=1 https://github.com/nyanmisaka/rk-mirrors.git rkrga && \
    meson setup rkrga rkrga_build \
      --prefix=/usr/local \
      --libdir=lib \
      --buildtype=release \
      --default-library=shared \
      -Dcpp_args=-fpermissive \
      -Dlibdrm=false \
      -Dlibrga_demo=false && \
    meson configure rkrga_build && \
    ninja -C rkrga_build install

RUN mkdir -p /root/dev && cd /root/dev && \
    git clone --depth=1 https://github.com/nyanmisaka/ffmpeg-rockchip.git ffmpeg && \
    cd ffmpeg && \
    ./configure --prefix=/usr/local --enable-gpl --enable-version3 --enable-libdrm --enable-rkmpp --enable-rkrga && \
    make -j $(nproc) && \
    make install

COPY ./src /root/dev/src

RUN cd /root/dev/src && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release .. && \
    make -j $(nproc) && \
    make install

FROM ubuntu:22.04

RUN sed -i 's@//ports.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list

RUN apt-get update

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata && \
    ln -fs /usr/share/zoneinfo/Asia/Shanghai /etc/localtime && \
    dpkg-reconfigure --frontend noninteractive tzdata

RUN apt-get -y install \
    libass9 \
    libfreetype6 \
    libgnutls30 \
    libmp3lame0 \
    libsdl2-2.0-0 \
    libva2 \
    libvdpau1 \
    libvorbis0a \
    libxcb1 \
    libxcb-shm0 \
    libxcb-xfixes0 \
    zlib1g \
    libopencv-dev

COPY --from=builder /usr/local /usr/local

ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

ENTRYPOINT ["/usr/local/bin/video_decoder"]

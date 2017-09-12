# PersonDetection
This repository is based on OpenCV 3.x and Luna and is aiming at detecting a person.

# How to build
## Build Steps
* step1: build or install FFmpeg
* step2: build or install OpenCV 3.x
* step3: build Luna
* step4: build PersonDetection

### STEP1: Build FFmpeg
When you build FFmpeg you have to be careful. If your target board is 32-bits, you must add the --enable-pic option.

```bash
cd ~/git
git clone https://github.com/FFmpeg/FFmpeg.git
cd FFmpeg
```

* For 32-bits processor like Raspberry Pi 2 or 3
```bash
./configure --enable-libfreetype --enable-gpl \
            --enable-nonfree --enable-libx264 --enable-shared --enable-pic
make
sudo make install
```

* For 64-bits processor like macOS, Linux
```bash
./configure --enable-libfreetype --enable-gpl \
            --enable-nonfree --enable-libx264 --enable-shared --enable-pic
make
sudo make install
```

### STEP2: Build OpenCV 3.x
```bash
cd ~/git
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir build
cd build
cmake ..
make
sudo make install
```

### STEP3: Build Luna
Luna needs curl and mbedTLS libraries because of supporting RESTful client using TLS.
So you have to build those first before building Luna.

```bash
cd ~/git/PersonDetection
git submodule init
git submodule update
cd ./dependency/Luna
git submodule init
git submodule update
```

* build curl and mbedTLS
```bash
cd ~/git/PersonDetection/dependency/Luna
git submodule init
git submodule update
cd dependency
mkdir build
cd build
cmake ..
make
```

* build Luna
```bash
cd ~/git/PersonDetection/dependency/Luna
mkdir build
cd build
cmake ..
make
```

### STEP4: Build PersonDetection
```bash
cd ~/git/PersonDetection
mkdir build
cd build
cmake ..
make
```

# How to run
Under construction


# Demo Videos
## Demo Video1
[![Watch the video](https://img.youtube.com/vi/IzlVuL8iwe4/0.jpg)](https://youtu.be/IzlVuL8iwe4)

## Demo Video2
[![Watch the video](https://img.youtube.com/vi/vt96ou7Z3K0/0.jpg)](https://youtu.be/vt96ou7Z3K0)

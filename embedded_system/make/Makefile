# 定义编译器和编译选项
CXX = /opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
CXXFLAGS = -g -I/mnt/dependency/opencv/include -I/mnt/dependency/include -I$(SRC_FOLDER) -I$(SRC_FOLDER)/utils -I$(SRC_FOLDER)/transmit_module -I$(SRC_FOLDER)/detection_module -I$(SRC_FOLDER)/camera_module -I/opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/aarch64-linux-gnu/include/c++/9
LDFLAGS = -Wl,-rpath-link,/usr/lib/board/lib \
        -lpthread \
        -Wl,-rpath-link,/opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/aarch64-linux-gnu/lib \
        -L/usr/lib/board/lib -lspcdev -lm \
        -Wl,-rpath-link,/usr/lib/board/lib/hbmedia \
        -Wl,-rpath-link,/usr/lib/board/lib/aarch64-linux-gnu \
        -L/mnt/dependency/opencv/lib/  -lopencv_world\
        -L/mnt/dependency/lib/hbbpu -lcnn_intf -lhlog -ldnn -lhbrt_bernoulli_aarch64 \
        -L/mnt/dependency/lib/hbbpu -ldnn \
		-L/mnt/dependency/lib -lcurl

# 使用pkg-config获取OpenCV的编译和链接标志
PKG_CFLAGS = $(shell pkg-config --cflags opencv)
PKG_LDFLAGS = $(shell pkg-config --libs opencv)

# 定义源文件和目标文件
SRC_FOLDER = /mnt/git_repo/embedded_system
OBJS = clip.o conf.o frame.o main.o detection.o camera.o alert.o video_upload.o

# 生成的可执行文件
TARGET = sys_test

# 规则：默认目标
all: $(TARGET)

# 规则：生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(PKG_LDFLAGS)

# 规则：编译所有OBJS
frame.o: $(SRC_FOLDER)/utils/frame.c
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

conf.o: $(SRC_FOLDER)/utils/conf.c
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

clip.o: $(SRC_FOLDER)/utils/clip.c
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

detection.o: $(SRC_FOLDER)/detection_module/hmh_fall_detection.c
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

camera.o: $(SRC_FOLDER)/camera_module/camera.c
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

main.o: $(SRC_FOLDER)/main.c
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

alert.o: $(SRC_FOLDER)/transmit_module/alert.cpp
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

video_upload.o: $(SRC_FOLDER)/transmit_module/video_upload.cpp
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

# 规则：清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

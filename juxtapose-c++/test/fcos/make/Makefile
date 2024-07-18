# 定义编译器和编译选项
CXX = /opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
CXXFLAGS = -std=c++11 -g -I/mnt/dependency/opencv/include -I/mnt/dependency/include -I$(SRC_FOLDER) -I/mnt/xtensor/include -I/mnt/xtl/include -I/mnt/xtensor/include/xtensor
LDFLAGS = -L/opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/aarch64-linux-gnu/lib \
          -L/mnt/dependency/opencv/lib \
          -L/mnt/dependency/lib/hbbpu -lcnn_intf -lhlog -ldnn -lhbrt_bernoulli_aarch64 \
          -lpthread -lopencv_world -Wl,-rpath-link,/opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/aarch64-linux-gnu/lib \
          -Wl,-rpath-link,/mnt/dependency/lib/hbbpu

# 使用pkg-config获取OpenCV的编译和链接标志
PKG_CFLAGS = $(shell pkg-config --cflags opencv)
PKG_LDFLAGS = $(shell pkg-config --libs opencv)

TEST_FILE=/mnt/git_repo/juxtapose-c++/test/fcos/test.cpp

# 定义源文件和目标文件
SRC_FOLDER = /mnt/git_repo/juxtapose-c++
SRCS = $(SRC_FOLDER)/utils.cpp $(SRC_FOLDER)/fcos.cpp $(TEST_FILE)
OBJS = utils.o fcos.o test.o

# 生成的可执行文件
TARGET = test_fcos

# 规则：默认目标
all: $(TARGET)

# 规则：生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(PKG_LDFLAGS)

# 规则：编译utils.cpp
utils.o: $(SRC_FOLDER)/utils.cpp
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

# 规则：编译rtmpose.cpp
fcos.o: $(SRC_FOLDER)/fcos.cpp
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

# 规则：编译test.cpp
test.o: $(TEST_FILE)
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

# 规则：清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

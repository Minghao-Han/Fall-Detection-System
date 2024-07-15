# 定义编译器和编译选项
CXX = /opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
CXXFLAGS = -g -I/mnt/git_repo/embedded_system/utils -I/mnt/dependency/include -I$(SRC_FOLDER)
LDFLAGS = -L/opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/aarch64-linux-gnu/lib -lpthread -lm -lrt -ldl \
		  -L/opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/aarch64-linux-gnu/lib/hbmedia -lspcdev -lm \
		  -L

# 定义源文件和目标文件
SRC_FOLDER = /mnt/git_repo/embedded_system/camera_module
SRCS = $(SRC_FOLDER)/test_camera.c $(SRC_FOLDER)/camera.c
OBJS = camera.o test_camera.o 

# 生成的可执行文件
TARGET = testcamera

# 规则：默认目标
all: $(TARGET)

# 规则：生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(PKG_LDFLAGS)

# 规则：编译camera.c
camera.o: $(SRC_FOLDER)/camera.c
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

# 规则：编译test_camera.c
test_camera.o: $(SRC_FOLDER)/test_camera.c
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@



# 规则：清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

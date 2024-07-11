# Makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I/mnt/juxtapose-c++/eigen-3.3.9 -I/mnt/juxtapose-c++/bytetrack-cpp/include -I/mnt/juxtapose-c++ -I/usr/include/opencv4
LDFLAGS =

# 目标文件目录
OBJDIR = /mnt/juxtapose-c++/bytetrack-cpp/target

# 源文件和目标文件
SRCDIR = /mnt/juxtapose-c++/bytetrack-cpp/src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# 静态库文件
TARGET_LIB = libbytetrack.a

# 可执行文件
# TARGET_EXE = my_program

# 默认目标
all: $(TARGET_LIB) $(TARGET_EXE)

# 生成静态库
$(TARGET_LIB): $(OBJECTS)
	ar rcs $(TARGET_LIB) $(OBJECTS)

# 生成可执行文件（可选）
# $(TARGET_EXE): $(OBJDIR)/main.o $(TARGET_LIB)
# 	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# 编译源文件
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -rf $(OBJDIR)/*.o $(TARGET_LIB) $(TARGET_EXE)

.PHONY: all clean

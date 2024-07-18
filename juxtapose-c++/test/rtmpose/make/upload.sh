#!/bin/bash

# 本地文件路径
LOCAL_FILE="./testrtmpose /mnt/git_repo/juxtapose-c++/rtmpose.cpp /mnt/git_repo/juxtapose-c++/test/rtmpose/testrtmpose.cpp"

# 远程服务器用户名和主机名
REMOTE_USER="sunrise"
REMOTE_HOST="rdkx3"

# 远程文件夹路径
REMOTE_DIR="~"

# 传输多个文件
scp $LOCAL_FILE $REMOTE_USER@$REMOTE_HOST:$REMOTE_DIR



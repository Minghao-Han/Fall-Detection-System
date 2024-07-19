SRC_FOLDER="/mnt/git_repo/embedded_system"
SRC_FILES="$SRC_FOLDER/utils/*.c \
$SRC_FOLDER/detection_module/*.cpp \
$SRC_FOLDER/camera_module/camera.c \
$SRC_FOLDER/main.c \
$SRC_FOLDER/conf.txt \
$SRC_FOLDER/transmit_module/*.cpp \
$SRC_FOLDER/transmit_module/*.cpp"

EXE_FILE="./sys_test"
scp $SRC_FILES $EXE_FILE sunrise@192.168.142.187:~/sys_test
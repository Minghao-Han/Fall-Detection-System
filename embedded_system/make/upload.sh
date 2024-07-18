SRC_FOLDER="/mnt/git_repo/embedded_system"
SRC_FILES="$SRC_FOLDER/utils/frame.c \
$SRC_FOLDER/utils/conf.c \
$SRC_FOLDER/utils/clip.c \
$SRC_FOLDER/detection_module/hmh_fall_detection.c \
$SRC_FOLDER/camera_module/camera.c \
$SRC_FOLDER/main.c \
$SRC_FOLDER/conf.txt \
$SRC_FOLDER/transmit_module/alert.cpp
$SRC_FOLDER/transmit_module/video_upload.cpp"

EXE_FILE="./sys_test"
scp $SRC_FILES $EXE_FILE sunrise@rdkx3:~/sys_test
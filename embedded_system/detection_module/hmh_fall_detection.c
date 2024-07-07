#include "fall_detection.h"
#include "thread_pool.h"

#define MODEL_PATH "mymodel/fall_detection_model.bin"

fall_detector_t *fall_detector_init(frame_t *camera_buf, sem_t *fall_sem,frame_buf_t *clip){
    fall_detector_t *fall_detector = (fall_detector_t *)malloc(sizeof(fall_detector_t));
    if(fall_detector == NULL){
        return NULL;
    }
    fall_detector->camera_buf = camera_buf;
    fall_detector->fall_sem = fall_sem;
    fall_detector->clip = clip;
    return fall_detector;

}
void fall_detector_start(fall_detector_t *fall_detector);
void fall_detector_destroy(fall_detector_t *fall_detector){
    
    free(fall_detector);
}
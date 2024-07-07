#ifndef _INFERENCE_HPP
#define _INFERENCE_HPP 1

#include "frame.h"
#include "hbDNNheader.h"

// !!! NOTICE: CAN NOT HAVE MORE THAN ONE INFERENCE WORKS AT THE SAME TIME, BECAUSE OF THE USE OF STATIC VARIABLE IN IMPLEMENT IN ORDER TO PERFORM THE REAL TIME TASK !!!
// !!! 注意：不能同时有两个inference任务，因为实现中为了更快的速度使用了static变量 !!!
#ifdef __cplusplus
extern "C"{
#endif

void inference_init(hbDNNInferCtrlParam *infer_ctrl_param,char *model_path);
void infer(frame_t *frame);
void inference_destroy();

#ifdef __cplusplus
}
#endif

#endif // _INFERENCE_HPP
#ifndef _INFERENCE_H
#define _INFERENCE_H 1

#include "frame.h"

void inference_init();
void inference_start(frame_t *frame);
void inference_destroy();


#endif // _INFERENCE_H
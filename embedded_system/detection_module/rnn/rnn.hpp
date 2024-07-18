#ifndef _RNN_HPP
#define _RNN_HPP 1
#include <semaphore.h>
#include <queue>
#include "dnn/hb_dnn.h"
#include "dnn/hb_sys.h"
#include "DetecResult.h"

class RNNDetector{
    public:
        RNNDetector(const char *model_path);
        ~RNNDetector(); //free output_properties dnn_handle

        void fall_detect(bool *continue_detect, sem_t *fall_sem, int idx,float threshold);

    private: 
        std::queue<DetectBox> inputss[4]; //至多同时检测4人
        hbPackedDNNHandle_t packed_dnn_handle;
        hbDNNHandle_t dnn_handle;
        hbDNNTensor input;
        hbDNNTensorProperties input_properties;
        hbDNNTensorProperties output_property;
        int model_output_count;
};
#endif // _RNN_HPP
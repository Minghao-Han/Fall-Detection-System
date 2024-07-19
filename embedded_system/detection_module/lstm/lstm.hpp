#ifndef _LSTM_HPP
#define _LSTM_HPP 1
#include <semaphore.h>
#include <queue>
#include "dnn/hb_dnn.h"
#include "dnn/hb_sys.h"
#include "DetecResult.h"

class LSTMDetector{
    public:
        LSTMDetector(const char *model_path);
        ~LSTMDetector(); //free output_properties dnn_handle

        void fall_detect(bool *continue_detect, sem_t *fall_sem, int idx,float threshold);

        std::queue<Result> inputss[4]; //至多同时检测4人    
    private: 
        hbPackedDNNHandle_t packed_dnn_handle;
        hbDNNHandle_t dnn_handle;
        hbDNNTensor input;
        hbDNNTensorProperties input_property;
        hbDNNTensorProperties hidden_property;
        hbDNNTensorProperties c_property;
        hbDNNTensorProperties output_property;
        int model_output_count;
};
#endif // _LSTM_HPP
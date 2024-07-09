#include <stdint.h>
#ifndef _HBDNNHEADER_H
#define _HBDNNHEADER_H 1
#define HB_DNN_TENSOR_MAX_DIMENSIONS 8

#define HB_DNN_INITIALIZE_INFER_CTRL_PARAM(param) \
{                                                 \
    (param)->bpuCoreId = HB_BPU_CORE_ANY;         \
    (param)->dspCoreId = HB_DSP_CORE_ANY;         \
    (param)->priority = HB_DNN_PRIORITY_LOWEST;   \
    (param)->more = false;                        \
    (param)->customId = 0;                        \
    (param)->reserved1 = 0;                       \
    (param)->reserved2 = 0;                       \
}
// int32_t hbDNNRegisterLayerCreator(const char *layerType,
//                                   hbDNNLayerCreator layerCreator);
// int32_t hbDNNUnregisterLayerCreator(const char *layerType);
// 错误码
#define HB_DNN_SUCCESS = 0                              // 执行成功
#define HB_DNN_INVALID_ARGUMENT = -6000001              // 非法参数
#define HB_DNN_INVALID_MODEL = -6000002                 // 非法模型
#define HB_DNN_MODEL_NUMBER_EXCEED_LIMIT = -6000003     // 模型个数超过限制
#define HB_DNN_INVALID_PACKED_DNN_HANDLE = -6000004     // 非法packed handle
#define HB_DNN_INVALID_DNN_HANDLE = -6000005            // 非法handle
#define HB_DNN_CAN_NOT_OPEN_FILE = -6000006             // 文件不存在
#define HB_DNN_OUT_OF_MEMORY = -6000007                 // 没有足够的内存
#define HB_DNN_TIMEOUT = -6000008                       // 超时
#define HB_DNN_TASK_NUM_EXCEED_LIMIT = -6000009         // 任务数量超限制
#define HB_DNN_TASK_BATCH_SIZE_EXCEED_LIMIT = -6000010  // 多任务处理数量超限制
#define HB_DNN_INVALID_TASK_HANDLE = -6000011           // 非法task handle
#define HB_DNN_RUN_TASK_FAILED = -6000012               // 任务执行失败
#define HB_DNN_MODEL_IS_RUNNING = -6000013              // 任务执行中
#define HB_DNN_INCOMPATIBLE_MODEL = -6000014            // 不兼容的模型
#define HB_DNN_API_USE_ERROR = -6000015                 // 接口使用错误

#define HB_SYS_SUCCESS = 0                              // 执行成功
#define HB_SYS_INVALID_ARGUMENT = -6000129              // 非法参数
#define HB_SYS_OUT_OF_MEMORY = -6000130                 // 没有足够的内存
#define HB_SYS_REGISTER_MEM_FAILED = -6000131           // 注册内存失败
// 常用环境变量
#define HB_DNN_LOG_LEVEL                // 设置日志等级。
#define HB_DNN_CONV_MAP_PATH            // 模型卷积层配置文件路径；编译参数layer_out_dump为true时产生的json文件。
#define HB_DNN_DUMP_PATH                // 模型卷积层结果输出路径，与HB_DNN_CONV_MAP_PATH配合使用。
#define HB_DNN_PLUGIN_PATH              // 自定义CPU算子动态链接库所在目录。
#define HB_DNN_PROFILER_LOG_PATH        // 模型运行各阶段耗时统计信息dump路径。
#define HB_DNN_SIM_PLATFORM             // x86模拟器模拟平台设置，可设置为BERNOULLI、BERNOULLI2、BAYES。
#define HB_DNN_SIM_BPU_MEM_SIZE         // x86模拟器设置BPU内存大小，单位MB。


typedef void *hbPackedDNNHandle_t;
typedef void *hbDNNHandle_t;
typedef void *hbDNNTaskHandle_t;
typedef void (*hbDNNTaskDoneCb)(hbDNNTaskHandle_t taskHandle, int32_t status,void *userdata);
typedef enum {
  HB_DNN_LAYOUT_NHWC = 0,
  HB_DNN_LAYOUT_NCHW = 2,
  HB_DNN_LAYOUT_NONE = 255,
} hbDNNTensorLayout;
typedef enum {
  HB_DNN_IMG_TYPE_Y,
  HB_DNN_IMG_TYPE_NV12,
  HB_DNN_IMG_TYPE_NV12_SEPARATE,
  HB_DNN_IMG_TYPE_YUV444,
  HB_DNN_IMG_TYPE_RGB,
  HB_DNN_IMG_TYPE_BGR,
  HB_DNN_TENSOR_TYPE_S4,
  HB_DNN_TENSOR_TYPE_U4,
  HB_DNN_TENSOR_TYPE_S8,
  HB_DNN_TENSOR_TYPE_U8,
  HB_DNN_TENSOR_TYPE_F16,
  HB_DNN_TENSOR_TYPE_S16,
  HB_DNN_TENSOR_TYPE_U16,
  HB_DNN_TENSOR_TYPE_F32,
  HB_DNN_TENSOR_TYPE_S32,
  HB_DNN_TENSOR_TYPE_U32,
  HB_DNN_TENSOR_TYPE_F64,
  HB_DNN_TENSOR_TYPE_S64,
  HB_DNN_TENSOR_TYPE_U64,
  HB_DNN_TENSOR_TYPE_MAX
} hbDNNDataType;
typedef struct {
  int32_t dimensionSize[HB_DNN_TENSOR_MAX_DIMENSIONS];
  int32_t numDimensions;
} hbDNNTensorShape;
typedef struct {
  int32_t shiftLen;
  uint8_t *shiftData;
} hbDNNQuantiShift;
typedef struct {
  int32_t scaleLen;
  float *scaleData;
  int32_t zeroPointLen;
  int8_t *zeroPointData;
} hbDNNQuantiScale;
typedef enum {
  NONE,
  SHIFT,
  SCALE,
} hbDNNQuantiType;
typedef struct {
  hbDNNTensorShape validShape;
  hbDNNTensorShape alignedShape;
  int32_t tensorLayout;
  int32_t tensorType;
  hbDNNQuantiShift shift;
  hbDNNQuantiScale scale;
  hbDNNQuantiType quantiType;
  int32_t quantizeAxis;
  int32_t alignedByteSize;
  int32_t stride[HB_DNN_TENSOR_MAX_DIMENSIONS];
} hbDNNTensorProperties;
typedef enum {
  HB_DNN_PRIORITY_LOWEST = 0,
  HB_DNN_PRIORITY_HIGHEST = 255,
  HB_DNN_PRIORITY_PREEMP = HB_DNN_PRIORITY_HIGHEST,
} hbDNNTaskPriority;
typedef struct {
  hbSysMem sysMem[4];
  hbDNNTensorProperties properties;
} hbDNNTensor;
typedef struct {
  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;
} hbDNNRoi;
typedef struct {
  int32_t bpuCoreId;
  int32_t dspCoreId;
  int32_t priority;
  int32_t more;
  int64_t customId;
  int32_t reserved1;
  int32_t reserved2;
} hbDNNInferCtrlParam;
typedef enum {
  HB_BPU_CORE_ANY = 0,
  HB_BPU_CORE_0 = (1 << 0),
  HB_BPU_CORE_1 = (1 << 1)
} hbBPUCore;
typedef enum {
  HB_DSP_CORE_ANY = 0,
  HB_DSP_CORE_0 = (1 << 0),
  HB_DSP_CORE_1 = (1 << 1)
} hbDSPCore;
typedef struct {
  uint64_t phyAddr;
  void *virAddr;
  uint32_t memSize;
} hbSysMem;
typedef enum {
  HB_SYS_MEM_CACHE_INVALIDATE = 1,
  HB_SYS_MEM_CACHE_CLEAN = 2
} hbSysMemFlushFlag;
typedef enum {
  HB_DNN_RESIZE_TYPE_BILINEAR = 0,
} hbDNNResizeType;
typedef struct {
  int32_t bpuCoreId;
  int32_t priority;
  hbDNNResizeType resizeType;
  int32_t reserved1;
  int32_t reserved2;
  int32_t reserved3;
  int32_t reserved4;
} hbDNNResizeCtrlParam;
// typedef hobot::dnn::Layer *(*hbDNNLayerCreator)();
const char *hbDNNGetVersion();
int32_t hbDNNInitializeFromFiles(hbPackedDNNHandle_t *packedDNNHandle,const char **modelFileNames,int32_t modelFileCount);
int32_t hbDNNInitializeFromDDR(hbPackedDNNHandle_t *packedDNNHandle,const void **modelData,int32_t *modelDataLengths,int32_t modelDataCount);
int32_t hbDNNRelease(hbPackedDNNHandle_t packedDNNHandle);
int32_t hbDNNGetModelNameList(const char ***modelNameList,int32_t *modelNameCount,hbPackedDNNHandle_t packedDNNHandle);
int32_t hbDNNGetModelHandle(hbDNNHandle_t *dnnHandle,hbPackedDNNHandle_t packedDNNHandle,const char *modelName);
int32_t hbDNNGetInputCount(int32_t *inputCount,hbDNNHandle_t dnnHandle);
int32_t hbDNNGetInputName(const char **name,hbDNNHandle_t dnnHandle,int32_t inputIndex);
int32_t hbDNNGetInputTensorProperties(hbDNNTensorProperties *properties,
                                      hbDNNHandle_t dnnHandle,
                                      int32_t inputIndex);
int32_t hbDNNGetOutputCount(int32_t *outputCount,
                            hbDNNHandle_t dnnHandle);
int32_t hbDNNGetOutputName(const char **name,
                           hbDNNHandle_t dnnHandle,
                           int32_t outputIndex);
int32_t hbDNNGetOutputTensorProperties(hbDNNTensorProperties *properties,
                                       hbDNNHandle_t dnnHandle,
                                       int32_t outputIndex);
int32_t hbDNNInfer(hbDNNTaskHandle_t *taskHandle,
                   hbDNNTensor **output,
                   const hbDNNTensor *input,
                   hbDNNHandle_t dnnHandle,
                   hbDNNInferCtrlParam *inferCtrlParam);
int32_t hbDNNRoiInfer(hbDNNTaskHandle_t *taskHandle,
                      hbDNNTensor **output,
                      const hbDNNTensor *input,
                      hbDNNRoi *rois,
                      int32_t roiCount,
                      hbDNNHandle_t dnnHandle,
                      hbDNNInferCtrlParam *inferCtrlParam);
int32_t hbDNNWaitTaskDone(hbDNNTaskHandle_t taskHandle,
                          int32_t timeout);
int32_t hbDNNReleaseTask(hbDNNTaskHandle_t taskHandle);
int32_t hbDNNSetTaskDoneCb(hbDNNTaskHandle_t taskHandle, hbDNNTaskDoneCb cb,
                           void *userdata);

int32_t hbSysAllocMem(hbSysMem *mem, uint32_t size);
int32_t hbSysAllocCachedMem(hbSysMem *mem, uint32_t size);
int32_t hbSysFlushMem(hbSysMem *mem, int32_t flag);
int32_t hbSysFreeMem(hbSysMem *mem);
int32_t hbSysWriteMem(hbSysMem *dest, char *src, uint32_t size);
int32_t hbSysReadMem(char *dest, hbSysMem *src, uint32_t size);
int32_t hbSysRegisterMem(hbSysMem *mem);
int32_t hbSysUnregisterMem(hbSysMem *mem);
int32_t hbDNNResize(hbDNNTaskHandle_t *taskHandle,
                    hbDNNTensor *output,
                    const hbDNNTensor *input,
                    const hbDNNRoi *roi,
                    hbDNNResizeCtrlParam *resizeCtrlParam);

#endif // _HBDNNHEADER_H
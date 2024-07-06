
#ifndef _HBDNNHEADER_H
#define _HBDNNHEADER_H 1
#include <stdint.h>
#define HB_DNN_TENSOR_MAX_DIMENSIONS 8
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

#endif // _HBDNNHEADER_H
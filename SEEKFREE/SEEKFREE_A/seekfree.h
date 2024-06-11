/*********************************************************************************************************************
* 修改记录
* 日期             作者             备注
* 2024-1-11        SeekFree         first version
* 2024-06-08       HzMi_ZGY         used for STM32
********************************************************************************************************************/
#ifndef _seekfree_h_
#define _seekfree_h_

#include "main.h"
#include "usart.h"
#include "stdio.h"
#include "debug.h"
#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define uint64 uint64_t

#define int8 int8_t
#define int16 int16_t
#define int32 int32_t
#define int64 int64_t
#define ZF_WEAK	__weak



#define DEBUG_USART	huart3
#define DEBUG_USART_IT	USART3

typedef volatile uint8      vuint8;                                             // 易变性修饰 无符号  8 bits
typedef volatile uint16     vuint16;                                            // 易变性修饰 无符号 16 bits
typedef volatile uint32     vuint32;                                            // 易变性修饰 无符号 32 bits
typedef volatile uint64     vuint64;                                            // 易变性修饰 无符号 64 bits

typedef volatile int8       vint8;                                              // 易变性修饰 有符号  8 bits
typedef volatile int16      vint16;                                             // 易变性修饰 有符号 16 bits
typedef volatile int32      vint32;                                             // 易变性修饰 有符号 32 bits
typedef volatile int64      vint64;                                             // 易变性修饰 有符号 64 bits
// 数据发送设备枚举
typedef enum
{
    SEEKFREE_ASSISTANT_DEBUG_UART,      // 调试串口    使用的串口由DEBUG_UART_INDEX宏定义指定
    SEEKFREE_ASSISTANT_WIRELESS_UART,   // 无线转串口
    SEEKFREE_ASSISTANT_CH9141,          // CH9141蓝牙
    SEEKFREE_ASSISTANT_WIFI_UART,       // WIFI转串口
    SEEKFREE_ASSISTANT_WIFI_SPI,        // 高速WIFI SPI
    SEEKFREE_ASSISTANT_CUSTOM,          // 自定义通讯方式 需要自行seekfree_assistant_transfer与seekfree_assistant_receive函数
} seekfree_assistant_transfer_device_enum;


void    seekfree_assistant_interface_init   (void);




// 1：使能参数调节的功能  0：关闭参数调节的功能
#define SEEKFREE_ASSISTANT_SET_PARAMETR_ENABLE      ( 1 )



// 定义接收FIFO大小
#define SEEKFREE_ASSISTANT_BUFFER_SIZE              ( 0x80 )

// 定义示波器的最大通道数量
#define SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT   ( 0x08 )

// 定义参数调试的最大通道数量
#define SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT       ( 0x08 )

// 定义图像边线最大数量
#define SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY      ( 0x08 )

// 单片机往上位机发送的帧头
#define SEEKFREE_ASSISTANT_SEND_HEAD                ( 0xAA )

// 摄像头类
#define SEEKFREE_ASSISTANT_CAMERA_FUNCTION          ( 0x02 )
#define SEEKFREE_ASSISTANT_CAMERA_DOT_FUNCTION      ( 0x03 )
#define SEEKFREE_ASSISTANT_CAMERA_OSCILLOSCOPE      ( 0x10 )

// 上位机往单片机发送的帧头
#define SEEKFREE_ASSISTANT_RECEIVE_HEAD             ( 0x55 )

// 参数设置类
#define SEEKFREE_ASSISTANT_RECEIVE_SET_PARAMETER    ( 0x20 )


// 摄像头类型枚举
typedef enum
{
    // 按照摄像头型号定义
    SEEKFREE_ASSISTANT_OV7725_BIN = 1,
    SEEKFREE_ASSISTANT_MT9V03X,
    SEEKFREE_ASSISTANT_SCC8660,

    // 按照图像类型定义
    SEEKFREE_ASSISTANT_BINARY = 1,
    SEEKFREE_ASSISTANT_GRAY,
    SEEKFREE_ASSISTANT_RGB565,
} seekfree_assistant_image_type_enum;

// 摄像头类型枚举
typedef enum
{
    // 按照摄像头型号定义
    X_BOUNDARY,     // 发送的图像中边界信息只包含X，也就是只有横坐标信息，纵坐标根据图像高度得到
    Y_BOUNDARY,     // 发送的图像中边界信息只包含Y，也就是只有纵坐标信息，横坐标根据图像宽度得到，通常很少有这样的需求
    XY_BOUNDARY,    // 发送的图像中边界信息包含X与Y，这样可以指定点在任意位置，就可以方便显示出回弯的效果
    NO_BOUNDARY,    // 发送的图像中没有边线信息
} seekfree_assistant_boundary_type_enum;

typedef struct
{
    uint8 head;                                                 // 帧头
    uint8 channel_num;                                          // 高四位为功能字  低四位为通道数量
    uint8 check_sum;                                            // 和校验
    uint8 length;                                               // 包长度
    float data[SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT];      // 通道数据
} seekfree_assistant_oscilloscope_struct;


typedef struct
{
    uint8 head;                                                 // 帧头
    uint8 function;                                             // 功能字
    uint8 camera_type;                                          // 低四位表示边界数量 第四位表示是否有图像数据  例如0x13：其中3表示一副图像有三条边界（通常是左边界、中线、右边界）、1表示没有图像数据
    uint8 length;                                               // 包长度（仅包含协议部分）
    uint16 image_width;                                         // 图像宽度
    uint16 image_height;                                        // 图像高度
} seekfree_assistant_camera_struct;


typedef struct
{
    uint8 head;                                                 // 帧头
    uint8 function;                                             // 功能字
    uint8 dot_type;                                             // 点类型  BIT5：1：坐标是16位的 0：坐标是8位的    BIT7-BIT6：0：只有X坐标 1：只有Y坐标 2：X和Y坐标都有    BIT3-BIT0：边界数量
    uint8 length;                                               // 包长度（仅包含协议部分）
    uint16 dot_num;                                             // 画点数量
    uint8  valid_flag;                                          // 通道标识
    uint8  reserve;                                             // 保留
} seekfree_assistant_camera_dot_struct;

typedef struct
{
    void *image_addr;                                           // 摄像头地址
    uint16 width;                                               // 图像宽度
    uint16 height;                                              // 图像高度
    seekfree_assistant_image_type_enum camera_type;             // 摄像头类型
    void *boundary_x[SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY];   // 边界横坐标数组地址
    void *boundary_y[SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY];   // 边界纵坐标数组地址
} seekfree_assistant_camera_buffer_struct;

typedef struct
{
    uint8 head;                                                 // 帧头
    uint8 function;                                             // 功能字
    uint8 channel;                                              // 通道
    uint8 check_sum;                                            // 和校验
    float data;                                                 // 数据
} seekfree_assistant_parameter_struct;

typedef uint32 (*seekfree_assistant_transfer_callback_function) (const uint8 *buff, uint32 length);
typedef uint32 (*seekfree_assistant_receive_callback_function)  (uint8 *buff, uint32 length);

extern seekfree_assistant_oscilloscope_struct                   seekfree_assistant_oscilloscope_data;                                               // 虚拟示波器数据
extern float                                                    seekfree_assistant_parameter[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT];                // 保存接收到的参数
extern vuint8                                                   seekfree_assistant_parameter_update_flag[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT];    // 参数更新标志位


void    seekfree_assistant_oscilloscope_send                    (seekfree_assistant_oscilloscope_struct *seekfree_assistant_oscilloscope);
void    seekfree_assistant_camera_information_config            (seekfree_assistant_image_type_enum camera_type, void *image_addr, uint16 width, uint16 height);
void    seekfree_assistant_camera_boundary_config               (seekfree_assistant_boundary_type_enum boundary_type, uint16 dot_num, void *dot_x1, void *dot_x2, void *dot_x3, void *dot_y1, void *dot_y2, void *dot_y3);
void    seekfree_assistant_camera_send                          (void);
void    seekfree_assistant_data_analysis                        (void);
uint32 debug_send_buffer(const uint8 *buff, uint32 len);
uint32 debug_read_ring_buffer (uint8 *buff, uint32 len);
void Debug_Receive(void);
#endif

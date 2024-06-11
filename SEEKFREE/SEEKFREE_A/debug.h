/*********************************************************************************************************************
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
* 2022-05-26       pudding            新增调试串口发送函数，修改默认开启接收中断
* 2022-05-27       pudding            新增四个总线报错接管，新增总线报错时关闭所有中断及PWM输出
* 2024-06-08       HzMi_ZGY         	used for STM32
********************************************************************************************************************/

#ifndef _debug_h_
#define _debug_h_

#include "main.h"
#include "seekfree.h"
#include "fifo.h"

#define DEBUG_RING_BUFFER_LEN          (64)     // 定义环形缓冲区大小 默认 64byte

extern uint8                       debug_uart_buffer[DEBUG_RING_BUFFER_LEN];           // 数据存放数组
extern uint8                       debug_uart_data;
extern fifo_struct                 debug_uart_fifo;


//#define DEBUG_UART_INDEX            (UART_0)            // 指定 debug uart 所使用的的串口
#define DEBUG_UART_BAUDRATE         (115200)            // 指定 debug uart 所使用的的串口波特率
//#define DEBUG_UART_TX_PIN           (UART0_TX_P14_0)    // 指定 debug uart 所使用的的串口引脚
//#define DEBUG_UART_RX_PIN           (UART0_RX_P14_1)    // 指定 debug uart 所使用的的串口引脚
#define DEBUG_UART_USE_INTERRUPT    (1)                 // 是否启用 debug uart 接收中断


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     断言
// 参数说明     x           判断是否触发断言 0-触发断言 1-不触发断言
// 返回参数     void
// 使用示例     zf_assert(0);
// 备注信息     一般用于参数判断 zf_assert(0) 就断言报错
//             默认情况下会在 Debug UART 输出
//             但如果使用开源库内屏幕接口初始化了屏幕 则会在屏幕上显示
//-------------------------------------------------------------------------------------------------------------------
#define zf_assert(x)                (debug_assert_handler((x), __FILE__, __LINE__))     // 断言 一般用于参数判断 zf_assert(0) 就断言报错
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     Log 信息输出
// 参数说明     x           判断是否触发输出 0-触发输出 1-不触发输出
// 参数说明     *str        需要输出的 Log 信息
// 返回参数     void
// 使用示例     zf_log(0, "Error");
// 备注信息     调试信息输出 用来做一些报错或者警告之类的输出
//             默认情况下会在 Debug UART 输出
//             但如果使用开源库内屏幕接口初始化了屏幕 则会在屏幕上显示
//-------------------------------------------------------------------------------------------------------------------
#define zf_log(x, str)              (debug_log_handler((x), (str), __FILE__, __LINE__)) // 调试信息输出 用来做一些报错或者警告之类的输出

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     CPU报错接管
// 返回参数     void
// 备注信息     当触发CPU报错时会通过log信息输出来提醒用户
//-------------------------------------------------------------------------------------------------------------------
#define IFX_CFG_CPU_TRAP_BE_HOOK(x)     zf_log(0, "Memory access failure or Use an uninitialized peripheral, Locate faults through debugging"); assert_interrupt_config(); while(1);
#define IFX_CFG_CPU_TRAP_IPE_HOOK(x)    zf_log(0, "Accessing an null address, Locate faults through debugging");                                assert_interrupt_config(); while(1);
#define IFX_CFG_CPU_TRAP_ASSERT_HOOK(x) zf_log(0, "Cpu Assertion error, Locate faults through debugging");                                      assert_interrupt_config(); while(1);
#define IFX_CFG_CPU_TRAP_CME_HOOK(x)    zf_log(0, "Context management error, Locate faults through debugging");                                 assert_interrupt_config(); while(1);
#define IFX_CFG_CPU_TRAP_IE_HOOK(x)     zf_log(0, "Instruction Error, Locate faults through debugging");                                        assert_interrupt_config(); while(1);
#define IFX_CFG_CPU_TRAP_MME_HOOK(x)    zf_log(0, "Memory management error, Locate faults through debugging");                                  assert_interrupt_config(); while(1);


typedef struct
{
    uint16 type_index;

    uint16 display_x_max;
    uint16 display_y_max;

    uint8 font_x_size;
    uint8 font_y_size;

    void (*output_uart)                (const char *str);
    void (*output_screen)              (uint16 x, uint16 y, const char *str);
    void (*output_screen_clear)        (void);
} debug_output_struct;









uint32  debug_send_buffer              (const uint8 *buff, uint32 len);                 // 调试串口缓冲区发送
void    debug_assert_enable            (void);
void    debug_assert_disable           (void);
void    debug_assert_handler           (uint8 pass, char *file, int line);
void    debug_log_handler              (uint8 pass, char *str, char *file, int line);
void    debug_output_struct_init       (debug_output_struct *info);
void    debug_output_init              (debug_output_struct *info);
void    debug_init                     (void);

#endif


/*********************************************************************************************************************
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
* 2022-05-26       pudding            新增调试串口发送函数
* 2024-06-08       HzMi_ZGY         	used for STM32
********************************************************************************************************************/

#include "fifo.h"
#include "usart.h"
#include "debug.h"

static debug_output_struct  debug_output_info;
static volatile uint8       zf_debug_init_flag = 0;
static volatile uint8       zf_debug_assert_enable = 1;

uint8                       debug_uart_buffer[DEBUG_RING_BUFFER_LEN];           // 数据存放数组
uint8                       debug_uart_data;
fifo_struct                 debug_uart_fifo;






//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 输出接口
// 参数说明     *type       log 类型
// 参数说明     *file       文件名
// 参数说明     line        目标行数
// 参数说明     *str        信息
// 返回参数     void
// 使用示例     debug_output("Log message", file, line, str);
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void debug_output (char *type, char *file, int line, char *str)
{
    char *file_str;
    vuint16 i = 0, j = 0;
    vint32 len_origin = 0;
    vint16 show_len = 0;
    vint16 show_line_index = 0;
    len_origin = strlen(file);

    char output_buffer[256];
    char file_path_buffer[64];

    if(debug_output_info.type_index)
    {
        debug_output_info.output_screen_clear();
    }

    if(zf_debug_init_flag)
    {
        if(debug_output_info.type_index)
        {
            // 需要分行将文件的路径和行数输出
            // <不输出完整路径 只输出一级目录 例如 src/main.c>
            // 输出 line : xxxx
            debug_output_info.output_screen(0, show_line_index ++, type);

            file_str = file;
            len_origin = strlen(file);
            show_len = (debug_output_info.display_x_max / debug_output_info.font_x_size);

            while(*file_str++ != '\0');

            // 只取一级目录 如果文件放在盘符根目录 或者 MDK 的工程根目录 就会直接输出当前目录
            for(j = 0; (j < 2) && (len_origin >= 0); len_origin --)             // 查找两个 '/'
            {
                file_str --;
                if((*file_str == '/') || (*file_str == 0x5C))
                {
                    j ++;
                }
            }

            // 文件路径保存到数组中
            if(len_origin >= 0)
            {
                file_str ++;
                sprintf(output_buffer, "file: %s", file_str);
            }
            else
            {
                if(0 == j)
                {
                    sprintf(output_buffer, "file: mdk/%s", file_str);
                }
                else
                {
                    sprintf(output_buffer, "file: %s", file_str);
                }
            }

            // 屏幕显示路径
            for(i = 0; i < ((strlen(output_buffer) / show_len) + 1); i ++)
            {
                for(j = 0; j < show_len; j ++)
                {
                    if(strlen(output_buffer) < (j + i * show_len))
                    {
                        break;
                    }
                    file_path_buffer[j] = output_buffer[j + i * show_len];
                }

                file_path_buffer[j] = '\0';                                     // 末尾添加\0

                debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, file_path_buffer);
            }

            // 屏幕显示行号
            sprintf(output_buffer, "line: %d", line);
            debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, output_buffer);

            // 屏幕显示 Log 如果有的话
            if(NULL != str)
            {
                for(i = 0; i < ((strlen(str) / show_len) + 1); i ++)
                {
                    for(j = 0; j < show_len; j ++)
                    {
                        if(strlen(str) < (j + i * show_len))
                        {
                            break;
                        }
                        file_path_buffer[j] = str[j + i * show_len];
                    }

                    file_path_buffer[j] = '\0';                                 // 末尾添加\0

                    debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, file_path_buffer);
                }
            }
        }
        else
        {
            char output_buffer[256];
            memset(output_buffer, 0, 256);
            debug_output_info.output_uart(type);
            if(NULL != str)
            {
                sprintf(output_buffer, "\r\nfile %s line %d: %s.\r\n", file, line, str);
            }
            else
            {
                sprintf(output_buffer, "\r\nfile %s line %d.\r\n", file, line);
            }
            debug_output_info.output_uart(output_buffer);
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 输出绑定信息初始化
// 参数说明     *info       debug 输出的信息结构体
// 返回参数     void
// 使用示例     debug_output_struct_init(info);
// 备注信息     这个函数一般不由用户调用
//-------------------------------------------------------------------------------------------------------------------
void debug_output_struct_init (debug_output_struct *info)
{
    info->type_index            = 0;                        // 屏幕信息输出标志

    info->display_x_max         = 0xFFFF;                   // 屏幕显示X轴最大值
    info->display_y_max         = 0xFFFF;                   // 屏幕显示Y轴最大值

    info->font_x_size           = 0xFF;                     // 字体X轴最大值
    info->font_y_size           = 0xFF;                     // 字体Y轴最大值

    info->output_uart           = NULL;                     // 输出调试信息的串口
    info->output_screen         = NULL;                     // 输出调试信息的屏幕
    info->output_screen_clear   = NULL;                     // 屏幕清屏
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 输出绑定初始化
// 参数说明     *info       debug 输出的信息结构体
// 返回参数     void
// 使用示例     debug_output_init(info);
// 备注信息     这个函数一般不由用户调用
//-------------------------------------------------------------------------------------------------------------------
void debug_output_init (debug_output_struct *info)
{
    debug_output_info.type_index            = info->type_index;

    debug_output_info.display_x_max         = info->display_x_max;
    debug_output_info.display_y_max         = info->display_y_max;

    debug_output_info.font_x_size           = info->font_x_size;
    debug_output_info.font_y_size           = info->font_y_size;

    debug_output_info.output_uart           = info->output_uart;
    debug_output_info.output_screen         = info->output_screen;
    debug_output_info.output_screen_clear   = info->output_screen_clear;

    zf_debug_init_flag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 串口初始化
// 参数说明     void
// 返回参数     void
// 使用示例     debug_init();
// 备注信息     开源库示例默认调用 但默认禁用中断接收
//-------------------------------------------------------------------------------------------------------------------
void debug_init (void)
{
    debug_output_struct info;                   // 声明调试信息结构体
    debug_output_struct_init(&info);            // 初始化断言调试信息

    debug_output_init(&info);
    // 条件编译 只有在启用串口中断才编译
    fifo_init(&debug_uart_fifo, FIFO_DATA_8BIT, debug_uart_buffer, DEBUG_RING_BUFFER_LEN);


}




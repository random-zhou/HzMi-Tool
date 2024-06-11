/*********************************************************************************************************************
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding            first version
* 2022-05-26       pudding            �������Դ��ڷ��ͺ���
* 2024-06-08       HzMi_ZGY         	used for STM32
********************************************************************************************************************/

#include "fifo.h"
#include "usart.h"
#include "debug.h"

static debug_output_struct  debug_output_info;
static volatile uint8       zf_debug_init_flag = 0;
static volatile uint8       zf_debug_assert_enable = 1;

uint8                       debug_uart_buffer[DEBUG_RING_BUFFER_LEN];           // ���ݴ������
uint8                       debug_uart_data;
fifo_struct                 debug_uart_fifo;






//-------------------------------------------------------------------------------------------------------------------
// �������     debug ����ӿ�
// ����˵��     *type       log ����
// ����˵��     *file       �ļ���
// ����˵��     line        Ŀ������
// ����˵��     *str        ��Ϣ
// ���ز���     void
// ʹ��ʾ��     debug_output("Log message", file, line, str);
// ��ע��Ϣ     ���������ļ��ڲ����� �û����ù�ע Ҳ�����޸�
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
            // ��Ҫ���н��ļ���·�����������
            // <���������·�� ֻ���һ��Ŀ¼ ���� src/main.c>
            // ��� line : xxxx
            debug_output_info.output_screen(0, show_line_index ++, type);

            file_str = file;
            len_origin = strlen(file);
            show_len = (debug_output_info.display_x_max / debug_output_info.font_x_size);

            while(*file_str++ != '\0');

            // ֻȡһ��Ŀ¼ ����ļ������̷���Ŀ¼ ���� MDK �Ĺ��̸�Ŀ¼ �ͻ�ֱ�������ǰĿ¼
            for(j = 0; (j < 2) && (len_origin >= 0); len_origin --)             // �������� '/'
            {
                file_str --;
                if((*file_str == '/') || (*file_str == 0x5C))
                {
                    j ++;
                }
            }

            // �ļ�·�����浽������
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

            // ��Ļ��ʾ·��
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

                file_path_buffer[j] = '\0';                                     // ĩβ���\0

                debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, file_path_buffer);
            }

            // ��Ļ��ʾ�к�
            sprintf(output_buffer, "line: %d", line);
            debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, output_buffer);

            // ��Ļ��ʾ Log ����еĻ�
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

                    file_path_buffer[j] = '\0';                                 // ĩβ���\0

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
// �������     debug �������Ϣ��ʼ��
// ����˵��     *info       debug �������Ϣ�ṹ��
// ���ز���     void
// ʹ��ʾ��     debug_output_struct_init(info);
// ��ע��Ϣ     �������һ�㲻���û�����
//-------------------------------------------------------------------------------------------------------------------
void debug_output_struct_init (debug_output_struct *info)
{
    info->type_index            = 0;                        // ��Ļ��Ϣ�����־

    info->display_x_max         = 0xFFFF;                   // ��Ļ��ʾX�����ֵ
    info->display_y_max         = 0xFFFF;                   // ��Ļ��ʾY�����ֵ

    info->font_x_size           = 0xFF;                     // ����X�����ֵ
    info->font_y_size           = 0xFF;                     // ����Y�����ֵ

    info->output_uart           = NULL;                     // ���������Ϣ�Ĵ���
    info->output_screen         = NULL;                     // ���������Ϣ����Ļ
    info->output_screen_clear   = NULL;                     // ��Ļ����
}

//-------------------------------------------------------------------------------------------------------------------
// �������     debug ����󶨳�ʼ��
// ����˵��     *info       debug �������Ϣ�ṹ��
// ���ز���     void
// ʹ��ʾ��     debug_output_init(info);
// ��ע��Ϣ     �������һ�㲻���û�����
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
// �������     debug ���ڳ�ʼ��
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     debug_init();
// ��ע��Ϣ     ��Դ��ʾ��Ĭ�ϵ��� ��Ĭ�Ͻ����жϽ���
//-------------------------------------------------------------------------------------------------------------------
void debug_init (void)
{
    debug_output_struct info;                   // ����������Ϣ�ṹ��
    debug_output_struct_init(&info);            // ��ʼ�����Ե�����Ϣ

    debug_output_init(&info);
    // �������� ֻ�������ô����жϲű���
    fifo_init(&debug_uart_fifo, FIFO_DATA_8BIT, debug_uart_buffer, DEBUG_RING_BUFFER_LEN);


}




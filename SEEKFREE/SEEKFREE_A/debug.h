/*********************************************************************************************************************
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding            first version
* 2022-05-26       pudding            �������Դ��ڷ��ͺ������޸�Ĭ�Ͽ��������ж�
* 2022-05-27       pudding            �����ĸ����߱���ӹܣ��������߱���ʱ�ر������жϼ�PWM���
* 2024-06-08       HzMi_ZGY         	used for STM32
********************************************************************************************************************/

#ifndef _debug_h_
#define _debug_h_

#include "main.h"
#include "seekfree.h"
#include "fifo.h"

#define DEBUG_RING_BUFFER_LEN          (64)     // ���廷�λ�������С Ĭ�� 64byte

extern uint8                       debug_uart_buffer[DEBUG_RING_BUFFER_LEN];           // ���ݴ������
extern uint8                       debug_uart_data;
extern fifo_struct                 debug_uart_fifo;


//#define DEBUG_UART_INDEX            (UART_0)            // ָ�� debug uart ��ʹ�õĵĴ���
#define DEBUG_UART_BAUDRATE         (115200)            // ָ�� debug uart ��ʹ�õĵĴ��ڲ�����
//#define DEBUG_UART_TX_PIN           (UART0_TX_P14_0)    // ָ�� debug uart ��ʹ�õĵĴ�������
//#define DEBUG_UART_RX_PIN           (UART0_RX_P14_1)    // ָ�� debug uart ��ʹ�õĵĴ�������
#define DEBUG_UART_USE_INTERRUPT    (1)                 // �Ƿ����� debug uart �����ж�


//-------------------------------------------------------------------------------------------------------------------
// �������     ����
// ����˵��     x           �ж��Ƿ񴥷����� 0-�������� 1-����������
// ���ز���     void
// ʹ��ʾ��     zf_assert(0);
// ��ע��Ϣ     һ�����ڲ����ж� zf_assert(0) �Ͷ��Ա���
//             Ĭ������»��� Debug UART ���
//             �����ʹ�ÿ�Դ������Ļ�ӿڳ�ʼ������Ļ �������Ļ����ʾ
//-------------------------------------------------------------------------------------------------------------------
#define zf_assert(x)                (debug_assert_handler((x), __FILE__, __LINE__))     // ���� һ�����ڲ����ж� zf_assert(0) �Ͷ��Ա���
//-------------------------------------------------------------------------------------------------------------------
// �������     Log ��Ϣ���
// ����˵��     x           �ж��Ƿ񴥷���� 0-������� 1-���������
// ����˵��     *str        ��Ҫ����� Log ��Ϣ
// ���ز���     void
// ʹ��ʾ��     zf_log(0, "Error");
// ��ע��Ϣ     ������Ϣ��� ������һЩ������߾���֮������
//             Ĭ������»��� Debug UART ���
//             �����ʹ�ÿ�Դ������Ļ�ӿڳ�ʼ������Ļ �������Ļ����ʾ
//-------------------------------------------------------------------------------------------------------------------
#define zf_log(x, str)              (debug_log_handler((x), (str), __FILE__, __LINE__)) // ������Ϣ��� ������һЩ������߾���֮������

//-------------------------------------------------------------------------------------------------------------------
// �������     CPU����ӹ�
// ���ز���     void
// ��ע��Ϣ     ������CPU����ʱ��ͨ��log��Ϣ����������û�
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









uint32  debug_send_buffer              (const uint8 *buff, uint32 len);                 // ���Դ��ڻ���������
void    debug_assert_enable            (void);
void    debug_assert_disable           (void);
void    debug_assert_handler           (uint8 pass, char *file, int line);
void    debug_log_handler              (uint8 pass, char *str, char *file, int line);
void    debug_output_struct_init       (debug_output_struct *info);
void    debug_output_init              (debug_output_struct *info);
void    debug_init                     (void);

#endif


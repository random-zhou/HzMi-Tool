/*********************************************************************************************************************
* �޸ļ�¼
* ����             ����             ��ע
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

typedef volatile uint8      vuint8;                                             // �ױ������� �޷���  8 bits
typedef volatile uint16     vuint16;                                            // �ױ������� �޷��� 16 bits
typedef volatile uint32     vuint32;                                            // �ױ������� �޷��� 32 bits
typedef volatile uint64     vuint64;                                            // �ױ������� �޷��� 64 bits

typedef volatile int8       vint8;                                              // �ױ������� �з���  8 bits
typedef volatile int16      vint16;                                             // �ױ������� �з��� 16 bits
typedef volatile int32      vint32;                                             // �ױ������� �з��� 32 bits
typedef volatile int64      vint64;                                             // �ױ������� �з��� 64 bits
// ���ݷ����豸ö��
typedef enum
{
    SEEKFREE_ASSISTANT_DEBUG_UART,      // ���Դ���    ʹ�õĴ�����DEBUG_UART_INDEX�궨��ָ��
    SEEKFREE_ASSISTANT_WIRELESS_UART,   // ����ת����
    SEEKFREE_ASSISTANT_CH9141,          // CH9141����
    SEEKFREE_ASSISTANT_WIFI_UART,       // WIFIת����
    SEEKFREE_ASSISTANT_WIFI_SPI,        // ����WIFI SPI
    SEEKFREE_ASSISTANT_CUSTOM,          // �Զ���ͨѶ��ʽ ��Ҫ����seekfree_assistant_transfer��seekfree_assistant_receive����
} seekfree_assistant_transfer_device_enum;


void    seekfree_assistant_interface_init   (void);




// 1��ʹ�ܲ������ڵĹ���  0���رղ������ڵĹ���
#define SEEKFREE_ASSISTANT_SET_PARAMETR_ENABLE      ( 1 )



// �������FIFO��С
#define SEEKFREE_ASSISTANT_BUFFER_SIZE              ( 0x80 )

// ����ʾ���������ͨ������
#define SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT   ( 0x08 )

// ����������Ե����ͨ������
#define SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT       ( 0x08 )

// ����ͼ������������
#define SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY      ( 0x08 )

// ��Ƭ������λ�����͵�֡ͷ
#define SEEKFREE_ASSISTANT_SEND_HEAD                ( 0xAA )

// ����ͷ��
#define SEEKFREE_ASSISTANT_CAMERA_FUNCTION          ( 0x02 )
#define SEEKFREE_ASSISTANT_CAMERA_DOT_FUNCTION      ( 0x03 )
#define SEEKFREE_ASSISTANT_CAMERA_OSCILLOSCOPE      ( 0x10 )

// ��λ������Ƭ�����͵�֡ͷ
#define SEEKFREE_ASSISTANT_RECEIVE_HEAD             ( 0x55 )

// ����������
#define SEEKFREE_ASSISTANT_RECEIVE_SET_PARAMETER    ( 0x20 )


// ����ͷ����ö��
typedef enum
{
    // ��������ͷ�ͺŶ���
    SEEKFREE_ASSISTANT_OV7725_BIN = 1,
    SEEKFREE_ASSISTANT_MT9V03X,
    SEEKFREE_ASSISTANT_SCC8660,

    // ����ͼ�����Ͷ���
    SEEKFREE_ASSISTANT_BINARY = 1,
    SEEKFREE_ASSISTANT_GRAY,
    SEEKFREE_ASSISTANT_RGB565,
} seekfree_assistant_image_type_enum;

// ����ͷ����ö��
typedef enum
{
    // ��������ͷ�ͺŶ���
    X_BOUNDARY,     // ���͵�ͼ���б߽���Ϣֻ����X��Ҳ����ֻ�к�������Ϣ�����������ͼ��߶ȵõ�
    Y_BOUNDARY,     // ���͵�ͼ���б߽���Ϣֻ����Y��Ҳ����ֻ����������Ϣ�����������ͼ���ȵõ���ͨ������������������
    XY_BOUNDARY,    // ���͵�ͼ���б߽���Ϣ����X��Y����������ָ����������λ�ã��Ϳ��Է�����ʾ�������Ч��
    NO_BOUNDARY,    // ���͵�ͼ����û�б�����Ϣ
} seekfree_assistant_boundary_type_enum;

typedef struct
{
    uint8 head;                                                 // ֡ͷ
    uint8 channel_num;                                          // ����λΪ������  ����λΪͨ������
    uint8 check_sum;                                            // ��У��
    uint8 length;                                               // ������
    float data[SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT];      // ͨ������
} seekfree_assistant_oscilloscope_struct;


typedef struct
{
    uint8 head;                                                 // ֡ͷ
    uint8 function;                                             // ������
    uint8 camera_type;                                          // ����λ��ʾ�߽����� ����λ��ʾ�Ƿ���ͼ������  ����0x13������3��ʾһ��ͼ���������߽磨ͨ������߽硢���ߡ��ұ߽磩��1��ʾû��ͼ������
    uint8 length;                                               // �����ȣ�������Э�鲿�֣�
    uint16 image_width;                                         // ͼ����
    uint16 image_height;                                        // ͼ��߶�
} seekfree_assistant_camera_struct;


typedef struct
{
    uint8 head;                                                 // ֡ͷ
    uint8 function;                                             // ������
    uint8 dot_type;                                             // ������  BIT5��1��������16λ�� 0��������8λ��    BIT7-BIT6��0��ֻ��X���� 1��ֻ��Y���� 2��X��Y���궼��    BIT3-BIT0���߽�����
    uint8 length;                                               // �����ȣ�������Э�鲿�֣�
    uint16 dot_num;                                             // ��������
    uint8  valid_flag;                                          // ͨ����ʶ
    uint8  reserve;                                             // ����
} seekfree_assistant_camera_dot_struct;

typedef struct
{
    void *image_addr;                                           // ����ͷ��ַ
    uint16 width;                                               // ͼ����
    uint16 height;                                              // ͼ��߶�
    seekfree_assistant_image_type_enum camera_type;             // ����ͷ����
    void *boundary_x[SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY];   // �߽�����������ַ
    void *boundary_y[SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY];   // �߽������������ַ
} seekfree_assistant_camera_buffer_struct;

typedef struct
{
    uint8 head;                                                 // ֡ͷ
    uint8 function;                                             // ������
    uint8 channel;                                              // ͨ��
    uint8 check_sum;                                            // ��У��
    float data;                                                 // ����
} seekfree_assistant_parameter_struct;

typedef uint32 (*seekfree_assistant_transfer_callback_function) (const uint8 *buff, uint32 length);
typedef uint32 (*seekfree_assistant_receive_callback_function)  (uint8 *buff, uint32 length);

extern seekfree_assistant_oscilloscope_struct                   seekfree_assistant_oscilloscope_data;                                               // ����ʾ��������
extern float                                                    seekfree_assistant_parameter[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT];                // ������յ��Ĳ���
extern vuint8                                                   seekfree_assistant_parameter_update_flag[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT];    // �������±�־λ


void    seekfree_assistant_oscilloscope_send                    (seekfree_assistant_oscilloscope_struct *seekfree_assistant_oscilloscope);
void    seekfree_assistant_camera_information_config            (seekfree_assistant_image_type_enum camera_type, void *image_addr, uint16 width, uint16 height);
void    seekfree_assistant_camera_boundary_config               (seekfree_assistant_boundary_type_enum boundary_type, uint16 dot_num, void *dot_x1, void *dot_x2, void *dot_x3, void *dot_y1, void *dot_y2, void *dot_y3);
void    seekfree_assistant_camera_send                          (void);
void    seekfree_assistant_data_analysis                        (void);
uint32 debug_send_buffer(const uint8 *buff, uint32 len);
uint32 debug_read_ring_buffer (uint8 *buff, uint32 len);
void Debug_Receive(void);
#endif

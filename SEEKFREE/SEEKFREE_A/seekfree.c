/*********************************************************************************************************************
* �޸ļ�¼
* ����             ����             ��ע
* 2024-1-11        SeekFree         first version
* 2024-06-08       HzMi_ZGY         used for STM32
********************************************************************************************************************/

#include "fifo.h"
#include "seekfree.h"


uint8_t RxData=0;
extern seekfree_assistant_transfer_callback_function   seekfree_assistant_transfer_callback;    // ���ݷ��ͺ���ָ��
extern seekfree_assistant_receive_callback_function    seekfree_assistant_receive_callback;     // ���ݽ��պ���ָ��


//-------------------------------------------------------------------------------------------------------------------
// �������     ������ַ��ͺ���
// ����˵��     *buff           ��Ҫ���͵����ݵ�ַ
// ����˵��     length          ��Ҫ���͵ĳ���
// ���ز���     uint32          ʣ��δ�������ݳ���
// ʹ��ʾ��
//-------------------------------------------------------------------------------------------------------------------
ZF_WEAK uint32 seekfree_assistant_transfer (const uint8 *buff, uint32 length)
{

    // ��ѡ���Զ���ͨѶ��ʽʱ ��Ҫ����������ݷ��͹���
    return length;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ֽ������ݺ���
// ����˵��     *buff           ��Ҫ���յ����ݵ�ַ
// ����˵��     length          Ҫ���յ�������󳤶�
// ���ز���     uint32          ���յ������ݳ���
// ʹ��ʾ��
//-------------------------------------------------------------------------------------------------------------------
ZF_WEAK uint32 seekfree_assistant_receive (uint8 *buff, uint32 length)
{
    // ��ѡ���Զ���ͨѶ��ʽʱ ��Ҫ����������ݽ��չ���
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ֽӿ� ��ʼ��
// ����˵��
// ���ز���     void
// ʹ��ʾ��     seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI); ʹ�ø���WIFI SPIģ����������շ�
// ��ע         ��Ҫ���е����豸�ĳ�ʼ��������ʹ������ת���ڽ������ݵ��շ�������Ҫ���е�������ת���ڵĳ�ʼ����Ȼ���ٵ���seekfree_assistant_interface_init���������ֵĽӿڳ�ʼ��
//-------------------------------------------------------------------------------------------------------------------
ZF_WEAK void seekfree_assistant_interface_init (void)
{
//    switch(transfer_device)
//    {
//        case SEEKFREE_ASSISTANT_DEBUG_UART:
//        {
    debug_init();
    seekfree_assistant_transfer_callback = debug_send_buffer;
    seekfree_assistant_receive_callback = debug_read_ring_buffer;
    HAL_UART_Receive_IT(&DEBUG_USART,&RxData,1);
//        }break;
//
//        case SEEKFREE_ASSISTANT_WIRELESS_UART:
//        {
//            seekfree_assistant_transfer_callback = wireless_uart_send_buffer;
//            seekfree_assistant_receive_callback = wireless_uart_read_buffer;
//        }break;
//
//        case SEEKFREE_ASSISTANT_CH9141:
//        {
//            seekfree_assistant_transfer_callback = bluetooth_ch9141_send_buffer;
//            seekfree_assistant_receive_callback = bluetooth_ch9141_read_buffer;
//        }break;
//
//        case SEEKFREE_ASSISTANT_WIFI_UART:
//        {
//            seekfree_assistant_transfer_callback = wifi_uart_send_buffer;
//            seekfree_assistant_receive_callback = wifi_uart_read_buffer;
//        }break;
//
//        case SEEKFREE_ASSISTANT_WIFI_SPI:
//        {
//            seekfree_assistant_transfer_callback = wifi_spi_send_buffer;
//            seekfree_assistant_receive_callback = wifi_spi_read_buffer;
//        }break;
////
//        case SEEKFREE_ASSISTANT_CUSTOM:
//        {
//            // �����Լ������� ����ʵ��seekfree_assistant_transfer��seekfree_assistant_receive������������ݵ��շ�
//
//        }break;
//    }
}

void Debug_Receive(void)
{

    fifo_write_buffer(&debug_uart_fifo, &RxData, 1);               // ���� FIFO

    HAL_UART_Receive_IT(&DEBUG_USART,&RxData,1);
//	printf("ok!\n");
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==DEBUG_USART_IT)
    {

        Debug_Receive();
    }
}
int fputc(int ch,FILE *f)
{
    //采用轮询方式发�??1字节数据，超时时间设置为无限等待
    HAL_UART_Transmit(&DEBUG_USART,(uint8_t *)&ch,1,HAL_MAX_DELAY);
    return ch;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ���Դ��ڷ��ͻ�����
// ����˵��     *buff       �������ݴ�ŵ�����ָ��
// ����˵��     len         ��Ҫ���͵ĳ���
// ���ز���     uint32      ʣ��δ���͵ĳ���
// ʹ��ʾ��
// ��ע��Ϣ     ��������Ҫ���� DEBUG_UART_USE_INTERRUPT �궨��ſ�ʹ��
//-------------------------------------------------------------------------------------------------------------------
uint32 debug_send_buffer(const uint8 *buff, uint32 len)
{

    HAL_UART_Transmit(&DEBUG_USART, buff, len,HAL_MAX_DELAY);
    return 0;
}

// �������     ��ȡ debug ���λ���������
// ����˵��     *buff       �������ݴ�ŵ�����ָ��
// ����˵��     len         ��Ҫ��ȡ�ĳ���
// ���ز���     uint32      �������ݵ�ʵ�ʳ���
// ʹ��ʾ��
// ��ע��Ϣ     ��������Ҫ���� DEBUG_UART_USE_INTERRUPT �궨��ſ�ʹ��
//-------------------------------------------------------------------------------------------------------------------
uint32 debug_read_ring_buffer (uint8 *buff, uint32 len)
{
    fifo_read_buffer(&debug_uart_fifo, buff, &len, FIFO_READ_AND_CLEAN);
    return len;
}

extern uint32 seekfree_assistant_transfer       (const uint8 *buff, uint32 length);
extern uint32 seekfree_assistant_receive        (uint8 *buff, uint32 length);

#if (1 == SEEKFREE_ASSISTANT_SET_PARAMETR_ENABLE)
#include "fifo.h"
static uint8        seekfree_assistant_buffer[SEEKFREE_ASSISTANT_BUFFER_SIZE];                                      // FIFO������
static fifo_struct  seekfree_assistant_fifo =                                                                       // FIFO�ṹ��
{
    .buffer    = seekfree_assistant_buffer,
    .execution = FIFO_IDLE,
    .type      = FIFO_DATA_8BIT,
    .head      = 0,
    .end       = 0,
    .size      = SEEKFREE_ASSISTANT_BUFFER_SIZE,
    .max       = SEEKFREE_ASSISTANT_BUFFER_SIZE,
};
#endif

static seekfree_assistant_camera_struct         seekfree_assistant_camera_data;                                     // ͼ����λ��Э������
static seekfree_assistant_camera_dot_struct     seekfree_assistant_camera_dot_data;                                 // ͼ����λ�����Э������
static seekfree_assistant_camera_buffer_struct  seekfree_assistant_camera_buffer;                                   // ͼ���Լ��߽绺������Ϣ

seekfree_assistant_transfer_callback_function   seekfree_assistant_transfer_callback = seekfree_assistant_transfer; // ���ݷ��ͺ���ָ��
seekfree_assistant_receive_callback_function    seekfree_assistant_receive_callback  = seekfree_assistant_receive;  // ���ݽ��պ���ָ��

seekfree_assistant_oscilloscope_struct          seekfree_assistant_oscilloscope_data;                               // ����ʾ��������
float   seekfree_assistant_parameter[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT] = {0};                                  // ������յ��Ĳ���
vuint8  seekfree_assistant_parameter_update_flag[SEEKFREE_ASSISTANT_SET_PARAMETR_COUNT] = {0};                      // �������±�־λ

//-------------------------------------------------------------------------------------------------------------------
// �������     ���������ͺ���
// ����˵��     *buffer         ��ҪУ������ݵ�ַ
// ����˵��     length          У�鳤��
// ���ز���     uint8           ��ֵ
// ʹ��ʾ��
//-------------------------------------------------------------------------------------------------------------------
static uint8 seekfree_assistant_sum (uint8 *buffer, uint32 length)
{
    uint8 temp_sum = 0;

    while(length--)
    {
        temp_sum += *buffer++;
    }

    return temp_sum;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������� ͼ���ͺ���
// ����˵��     camera_type     ����ͷ����
// ����˵��     *image_addr     ͼ���׵�ַ
// ����˵��     boundary_num    ͼ���а����߽�����
// ����˵��     width           ͼ����
// ����˵��     height          ͼ��߶�
// ���ز���     void
// ʹ��ʾ��
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_camera_data_send (seekfree_assistant_image_type_enum camera_type, void *image_addr, uint8 boundary_num, uint16 width, uint16 height)
{
    uint32 image_size = 0;

    seekfree_assistant_camera_data.head           = SEEKFREE_ASSISTANT_SEND_HEAD;
    seekfree_assistant_camera_data.function       = SEEKFREE_ASSISTANT_CAMERA_FUNCTION;
    seekfree_assistant_camera_data.camera_type    = (camera_type << 5) | ((image_addr != NULL ? 0 : 1) << 4) | boundary_num;
    // д���������Ϣ��������Э�鲿��
    seekfree_assistant_camera_data.length         = sizeof(seekfree_assistant_camera_struct);
    seekfree_assistant_camera_data.image_width    = width;
    seekfree_assistant_camera_data.image_height   = height;

    // ���ȷ���֡ͷ�����ܡ�����ͷ���͡��Լ���ȸ߶ȵ���Ϣ
    seekfree_assistant_transfer_callback((const uint8 *)&seekfree_assistant_camera_data, sizeof(seekfree_assistant_camera_struct));

    // ��������ͷ���ͼ���ͼ���С
    switch(camera_type)
    {
    case SEEKFREE_ASSISTANT_OV7725_BIN:
    {
        image_size = width * height / 8;
    }
    break;

    case SEEKFREE_ASSISTANT_MT9V03X:
    {
        image_size = width * height;
    }
    break;

    case SEEKFREE_ASSISTANT_SCC8660:
    {
        image_size = width * height * 2;
    }
    break;
    }

    // ����ͼ������
    if(NULL != image_addr)
    {
        seekfree_assistant_transfer_callback(image_addr, image_size);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������� ͼ����߻��ƺ���
// ����˵��     boundary_id     ����ID
// ����˵��     dot_num         ������
// ����˵��     *dot_x          �����������׵�ַ
// ����˵��     *dot_y          �����������׵�ַ
// ����˵��     width           ͼ����
// ����˵��     height          ͼ��߶�
// ���ز���     void
// ʹ��ʾ��
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_camera_dot_send (seekfree_assistant_camera_buffer_struct *buffer)
{
    uint8  i;
    uint16 dot_bytes = 0;   // ���ֽ�����

    dot_bytes = seekfree_assistant_camera_dot_data.dot_num;

    if(seekfree_assistant_camera_dot_data.dot_type & (1 << 5))
    {
        dot_bytes *= 2;
    }

    // ���ȷ���֡ͷ�����ܡ��߽��š����곤�ȡ������
    seekfree_assistant_transfer_callback((const uint8 *)&seekfree_assistant_camera_dot_data, sizeof(seekfree_assistant_camera_dot_struct));

    for(i=0; i < SEEKFREE_ASSISTANT_CAMERA_MAX_BOUNDARY; i++)
    {
        // �ж��Ƿ��ͺ���������
        if(NULL != buffer->boundary_x[i])
        {
            seekfree_assistant_transfer_callback((const uint8 *)buffer->boundary_x[i], dot_bytes);
        }

        // �ж��Ƿ�������������
        if(NULL != buffer->boundary_y[i])
        {
            // ���û�����������ݣ����ʾÿһ��ֻ��һ���߽�
            // ָ���˺����������ݣ����ַ�ʽ����ʵ��ͬһ�ж���߽����������������㷨�ܹ����������䡣
            seekfree_assistant_transfer_callback((const uint8 *)buffer->boundary_y[i], dot_bytes);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������� ����ʾ�������ͺ���
// ����˵��     *seekfree_assistant_oscilloscope  ʾ�������ݽṹ��
// ���ز���     void
// ʹ��ʾ��     seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_oscilloscope_send (seekfree_assistant_oscilloscope_struct *seekfree_assistant_oscilloscope)
{
    uint8 packet_size;

    // ������λ���
    seekfree_assistant_oscilloscope->channel_num &= 0x0f;

//    zf_assert(SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT >= seekfree_assistant_oscilloscope->channel_num);

    // ֡ͷ
    seekfree_assistant_oscilloscope->head         = SEEKFREE_ASSISTANT_SEND_HEAD;

    // д���������Ϣ
    packet_size                         = sizeof(seekfree_assistant_oscilloscope_struct) - (SEEKFREE_ASSISTANT_SET_OSCILLOSCOPE_COUNT - seekfree_assistant_oscilloscope->channel_num) * 4;
    seekfree_assistant_oscilloscope->length       = packet_size;

    // д�빦������ͨ������
    seekfree_assistant_oscilloscope->channel_num |= SEEKFREE_ASSISTANT_CAMERA_OSCILLOSCOPE;

    // ��У�����
    seekfree_assistant_oscilloscope->check_sum    = 0;
    seekfree_assistant_oscilloscope->check_sum    = seekfree_assistant_sum((uint8 *)seekfree_assistant_oscilloscope, packet_size);

    // �����ڵ��ñ�����֮ǰ�����û�����Ҫ���͵�����д��seekfree_assistant_oscilloscope_data.data[]

    seekfree_assistant_transfer_callback((const uint8 *)seekfree_assistant_oscilloscope, packet_size);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �������ͼ����Ϣ���ú���
// ����˵��     camera_type     ͼ������
// ����˵��     image_addr      ͼ���ַ    �������NULL�������ʾֻ���ͱ�����Ϣ����λ��
// ����˵��     width           ͼ����
// ����˵��     height          ͼ��߶�
// ���ز���     void
// ʹ��ʾ��                     seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_camera_information_config (seekfree_assistant_image_type_enum camera_type, void *image_addr, uint16 width, uint16 height)
{
    seekfree_assistant_camera_dot_data.head       = SEEKFREE_ASSISTANT_SEND_HEAD;
    seekfree_assistant_camera_dot_data.function   = SEEKFREE_ASSISTANT_CAMERA_DOT_FUNCTION;
    // д���������Ϣ
    seekfree_assistant_camera_dot_data.length     = sizeof(seekfree_assistant_camera_dot_struct);

    seekfree_assistant_camera_buffer.camera_type  = camera_type;
    seekfree_assistant_camera_buffer.image_addr   = image_addr;
    seekfree_assistant_camera_buffer.width        = width;
    seekfree_assistant_camera_buffer.height       = height;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �������ͼ����߷������ú���
// ����˵��     boundary_type   �߽�����
// ����˵��     dot_num         һ���߽��ж��ٸ���
// ����˵��     dot_x1          ��ű���1������ĵ�ַ  �������NULL�������ʾ�����ͱ���1
// ����˵��     dot_x2          ��ű���2������ĵ�ַ  �������NULL�������ʾ�����ͱ���2
// ����˵��     dot_x3          ��ű���3������ĵ�ַ  �������NULL�������ʾ�����ͱ���3
// ����˵��     dot_y1          ��ű���1������ĵ�ַ  �������NULL�������ʾ�����ͱ���1
// ����˵��     dot_y2          ��ű���2������ĵ�ַ  �������NULL�������ʾ�����ͱ���2
// ����˵��     dot_y3          ��ű���3������ĵ�ַ  �������NULL�������ʾ�����ͱ���3
// ���ز���     void
// ʹ��ʾ��                     seekfree_assistant_camera_config(X_BOUNDARY, MT9V03X_H, x1_boundary, x2_boundary, x3_boundary, NULL, NULL, NULL);     // ͼ����ʱ�����������ߣ�����ֻ�к�����
// ʹ��ʾ��                     seekfree_assistant_camera_config(Y_BOUNDARY, MT9V03X_W, NULL, NULL, NULL, y1_boundary, y2_boundary, y3_boundary);     // ͼ����ʱ�����������ߣ�����ֻ��������
// ʹ��ʾ��                     seekfree_assistant_camera_config(XY_BOUNDARY, 160, xy_x1_boundary, xy_x2_boundary, xy_x3_boundary, xy_y1_boundary, xy_y2_boundary, xy_y3_boundary);   // ͼ����ʱ�����������ߣ����߰�����������
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_camera_boundary_config (seekfree_assistant_boundary_type_enum boundary_type, uint16 dot_num, void *dot_x1, void *dot_x2, void *dot_x3, void *dot_y1, void *dot_y2, void *dot_y3)
{
    uint8 i = 0;
    uint8 boundary_num = 0;
    uint8 boundary_data_type = 0;

    // ���ͼ���ͻ������Ƿ�׼������, ���ô˺���֮ǰ��Ҫ�ȵ���seekfree_assistant_camera_config���ú�ͼ����Ϣ
//    zf_assert(0 != seekfree_assistant_camera_buffer.camera_type);

    seekfree_assistant_camera_dot_data.dot_num    = dot_num;
    seekfree_assistant_camera_dot_data.valid_flag = 0;
    for(i = 0; i < 3; i++)
    {
        seekfree_assistant_camera_buffer.boundary_x[i] = NULL;
        seekfree_assistant_camera_buffer.boundary_y[i] = NULL;
    }

    switch(boundary_type)
    {
    case X_BOUNDARY:
    {
        if(NULL != dot_x1)
        {
            boundary_num++;
            seekfree_assistant_camera_dot_data.valid_flag |= 1 << 0;
            seekfree_assistant_camera_buffer.boundary_x[i++] = dot_x1;
        }
        if(NULL != dot_x2)
        {
            boundary_num++;
            seekfree_assistant_camera_dot_data.valid_flag |= 1 << 1;
            seekfree_assistant_camera_buffer.boundary_x[i++] = dot_x2;
        }
        if(NULL != dot_x3)
        {
            boundary_num++;
            seekfree_assistant_camera_dot_data.valid_flag |= 1 << 2;
            seekfree_assistant_camera_buffer.boundary_x[i++] = dot_x3;
        }

        if(255 < seekfree_assistant_camera_buffer.height)
        {
            boundary_data_type = 1;
        }
    }
    break;

    case Y_BOUNDARY:
    {
        if(NULL != dot_y1)
        {
            boundary_num++;
            seekfree_assistant_camera_dot_data.valid_flag |= 1 << 0;
            seekfree_assistant_camera_buffer.boundary_y[i++] = dot_y1;
        }
        if(NULL != dot_y2)
        {
            boundary_num++;
            seekfree_assistant_camera_dot_data.valid_flag |= 1 << 1;
            seekfree_assistant_camera_buffer.boundary_y[i++] = dot_y2;
        }
        if(NULL != dot_y3)
        {
            boundary_num++;
            seekfree_assistant_camera_dot_data.valid_flag |= 1 << 2;
            seekfree_assistant_camera_buffer.boundary_y[i++] = dot_y3;
        }

        if(255 < seekfree_assistant_camera_buffer.width)
        {
            boundary_data_type = 1;
        }
    }
    break;

    case XY_BOUNDARY:
    {
        if((NULL != dot_x1) && (NULL != dot_y1))
        {
            boundary_num++;
            seekfree_assistant_camera_dot_data.valid_flag |= 1 << 0;
            seekfree_assistant_camera_buffer.boundary_x[i]   = dot_x1;
            seekfree_assistant_camera_buffer.boundary_y[i++] = dot_y1;
        }
        if((NULL != dot_x2) && (NULL != dot_y2))
        {
            boundary_num++;
            seekfree_assistant_camera_dot_data.valid_flag |= 1 << 1;
            seekfree_assistant_camera_buffer.boundary_x[i]   = dot_x2;
            seekfree_assistant_camera_buffer.boundary_y[i++] = dot_y2;
        }
        if((NULL != dot_x3) && (NULL != dot_y3))
        {
            boundary_num++;
            seekfree_assistant_camera_dot_data.valid_flag |= 1 << 2;
            seekfree_assistant_camera_buffer.boundary_x[i]   = dot_x3;
            seekfree_assistant_camera_buffer.boundary_y[i++] = dot_y3;
        }

        if((255 < seekfree_assistant_camera_buffer.width) || (255 < seekfree_assistant_camera_buffer.height))
        {
            boundary_data_type = 1;
        }
    }
    break;

    case NO_BOUNDARY:
        break;
    }

    seekfree_assistant_camera_dot_data.dot_type   = (boundary_type << 6) | (boundary_data_type << 5) | boundary_num;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ������ַ�������ͷͼ��
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��
// ��ע��Ϣ     �ڵ���ͼ���ͺ���֮ǰ������ص���һ��seekfree_assistant_camera_config����������Ӧ�Ĳ������ú�
//-------------------------------------------------------------------------------------------------------------------
void seekfree_assistant_camera_send (void)
{
    // ���ͼ���ͻ������Ƿ�׼������
//    zf_assert(0 != seekfree_assistant_camera_buffer.camera_type);

    seekfree_assistant_camera_data_send(seekfree_assistant_camera_buffer.camera_type, seekfree_assistant_camera_buffer.image_addr, seekfree_assistant_camera_dot_data.dot_type & 0x0f, seekfree_assistant_camera_buffer.width, seekfree_assistant_camera_buffer.height);

    if(seekfree_assistant_camera_dot_data.dot_type & 0x0f)
    {
        seekfree_assistant_camera_dot_send(&seekfree_assistant_camera_buffer);
    }
}


//-------------------------------------------------------------------------------------------------------------------
// �������     ������ֽ������յ�������
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     ����ֻ��Ҫ�ŵ��������е�PIT�жϻ�����ѭ������
//-------------------------------------------------------------------------------------------------------------------
#if (1 == SEEKFREE_ASSISTANT_SET_PARAMETR_ENABLE)
void seekfree_assistant_data_analysis (void)
{
    uint8  temp_sum;
    uint32 read_length;
    seekfree_assistant_parameter_struct *receive_packet;

    // ����ʹ��uint32���ж��壬Ŀ����Ϊ�˱�֤�������ֽڶ���
    uint32  temp_buffer[SEEKFREE_ASSISTANT_BUFFER_SIZE / 4];

    // ���Զ�ȡ����, ��������Զ���Ĵ��䷽ʽ��ӽ��ջص��ж�ȡ����
    read_length = seekfree_assistant_receive_callback((uint8 *)temp_buffer, SEEKFREE_ASSISTANT_BUFFER_SIZE);
    if(read_length)
    {
        // ����ȡ��������д��FIFO
        fifo_write_buffer(&seekfree_assistant_fifo, (uint8 *)temp_buffer, read_length);
    }

    while(sizeof(seekfree_assistant_parameter_struct) <= fifo_used(&seekfree_assistant_fifo))
    {
        read_length = sizeof(seekfree_assistant_parameter_struct);
        fifo_read_buffer(&seekfree_assistant_fifo, (uint8 *)temp_buffer, &read_length, FIFO_READ_ONLY);

        if(SEEKFREE_ASSISTANT_RECEIVE_HEAD != ((uint8 *)temp_buffer)[0])
        {
            // û��֡ͷ���FIFO��ȥ����һ������
            read_length = 1;
        }
        else
        {
            // �ҵ�֡ͷ
            receive_packet = (seekfree_assistant_parameter_struct *)temp_buffer;
            temp_sum = receive_packet->check_sum;
            receive_packet->check_sum = 0;
            if(temp_sum == seekfree_assistant_sum((uint8 *)temp_buffer, sizeof(seekfree_assistant_parameter_struct)))
            {
                // ��У��ɹ���������
                seekfree_assistant_parameter[receive_packet->channel - 1] = receive_packet->data;
                seekfree_assistant_parameter_update_flag[receive_packet->channel - 1] = 1;
            }
            else
            {
                read_length = 1;
            }
        }

        // ��������ʹ�õ�����
        fifo_read_buffer(&seekfree_assistant_fifo, (uint8 *)temp_buffer, &read_length, FIFO_READ_AND_CLEAN);
    }
}
#endif



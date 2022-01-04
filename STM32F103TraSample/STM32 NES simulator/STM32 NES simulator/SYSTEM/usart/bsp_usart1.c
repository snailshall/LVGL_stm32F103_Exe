#include "bsp_usart1.h"
//#include "includes.h"


// ����USART1�����ж�
void NVIC_USART1_Config ( void )
{
    NVIC_InitTypeDef NVIC_InitBlock; 
    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the USARTy Interrupt */
    NVIC_InitBlock.NVIC_IRQChannel = USART1_IRQ;
    NVIC_InitBlock.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitBlock.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitBlock.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitBlock);
}

 /**
  * @brief  USARTx GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */
void USART1_Config ( void )
{
    GPIO_InitTypeDef GPIO_InitBlock;
    USART_InitTypeDef USART_InitBlock;

    /* config USART1 clock */
    USART1_APBxClock_FUN(USART1_CLK, ENABLE);
    USART1_GPIO_APBxClock_FUN(USART1_GPIO_CLK, ENABLE);

    /* config USART1 GPIO */
    GPIO_InitBlock.GPIO_Speed                  = GPIO_Speed_50MHz;

    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitBlock.GPIO_Pin                    = USART1_TX_PIN;
    GPIO_InitBlock.GPIO_Mode                   = GPIO_Mode_AF_PP;
    GPIO_Init(USART1_TX_PORT, &GPIO_InitBlock);

    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitBlock.GPIO_Pin                    = USART1_RX_PIN;
    GPIO_InitBlock.GPIO_Mode                   = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART1_RX_PORT, &GPIO_InitBlock);

    /* config USART1 mode */
    USART_InitBlock.USART_BaudRate             = USART1_BAUD_RATE;
    USART_InitBlock.USART_WordLength           = USART_WordLength_8b;
    USART_InitBlock.USART_StopBits             = USART_StopBits_1;
    USART_InitBlock.USART_Parity               = USART_Parity_No;
    USART_InitBlock.USART_HardwareFlowControl  = USART_HardwareFlowControl_None;
    USART_InitBlock.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitBlock);

#if !USE_USART_DMA

    /* ʹ�ܴ���1�����ж� */
    NVIC_USART1_Config();
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

#else

    // DMA ��ʽ��������
    NVIC_USART1_Config();
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    // ��������DMA����
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

#endif

    USART_Cmd(USART1, ENABLE);
}

#if USE_USART_DMA
char USART_RX_Buffer[USART_RECV_BUFFER_SIZE];

void USARTx_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitBlock;

    // ����DMAʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // ����DMAԴ��ַ���������ݼĴ�����ַ*/
    DMA_InitBlock.DMA_PeripheralBaseAddr = (uint32_t)USART_DR_ADDRESS;
    // �ڴ��ַ(Ҫ����ı�����ָ��)
    DMA_InitBlock.DMA_MemoryBaseAddr = (uint32_t)USART_RX_Buffer;
    // ���򣺴��ڴ浽����
    DMA_InitBlock.DMA_DIR = DMA_DIR_PeripheralSRC;
    // �����С
    DMA_InitBlock.DMA_BufferSize = USART_RECV_BUFFER_SIZE;
    // �����ַ����
    DMA_InitBlock.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // �ڴ��ַ����
    DMA_InitBlock.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // �������ݵ�λ
    DMA_InitBlock.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    // �ڴ����ݵ�λ
    DMA_InitBlock.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    // DMAģʽ��һ�λ���ѭ��ģʽ
    // DMA_InitBlock.DMA_Mode = DMA_Mode_Normal ;
    DMA_InitBlock.DMA_Mode = DMA_Mode_Circular;
    // ���ȼ�����
    DMA_InitBlock.DMA_Priority = DMA_Priority_VeryHigh;
    // ��ֹ�ڴ浽�ڴ�Ĵ���
    DMA_InitBlock.DMA_M2M = DMA_M2M_Disable;
    // ����DMAͨ��
    DMA_Init(USART_RX_DMA_CHANNEL, &DMA_InitBlock);
    // ���DMA���б�־
    DMA_ClearFlag(DMA1_FLAG_GL5);
    //DMA_ITConfig(USART_RX_DMA_CHANNEL, DMA_IT_TE, ENABLE);
    DMA_ITConfig(USART_RX_DMA_CHANNEL, DMA_IT_TC, ENABLE);
    // ʹ��DMA
    DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);
}
#endif

void USART1_Init ( void )
{
    USART1_Config ();
#if USE_USART_DMA
    USARTx_DMA_Config ();
#endif
}

#if !USE_USART_DMA
// �жϴ����������ַ���ʽ��������
void USART1_Recv_Handler ( void )
{
    char *p_mem_blk;


    if ( USART_GetITStatus ( USART1, USART_IT_RXNE ) != RESET )
    {
        *p_mem_blk = USART_ReceiveData ( USART1 );


    }
}
#else
// �жϴ�������DMA buffer��ʽ��������
void USART1_Recv_Handler ( void )
{
    if ( USART_GetITStatus( USART1, USART_IT_IDLE ) != RESET )
    {
        DMA_Cmd(USART_RX_DMA_CHANNEL, DISABLE);

        USART_ReceiveData( USART1 );  /* �����־λ */

        // ��DMA��־λ
        DMA_ClearITPendingBit( DMA1_IT_TC5 );
        // ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
        DMA_SetCurrDataCounter(USART_RX_DMA_CHANNEL, USART_RECV_BUFFER_SIZE);
        DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);

    }
}
#endif

// �ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
    /* ����һ���ֽ����ݵ�USART1 */
    USART_SendData(USART1, (uint8_t) ch);

    /* �ȴ�������� */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;

    return (ch);
}

// �ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
    /* �ȴ�����1�������� */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;

    return (int)USART_ReceiveData(USART1);
}

/*********************************************END OF FILE**********************/

#include "bsp_usart1.h"
//#include "includes.h"


// 配置USART1接收中断
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
  * @brief  USARTx GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
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

    /* 使能串口1接收中断 */
    NVIC_USART1_Config();
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

#else

    // DMA 方式接收数据
    NVIC_USART1_Config();
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    // 开启串口DMA接收
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

#endif

    USART_Cmd(USART1, ENABLE);
}

#if USE_USART_DMA
char USART_RX_Buffer[USART_RECV_BUFFER_SIZE];

void USARTx_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitBlock;

    // 开启DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 设置DMA源地址：串口数据寄存器地址*/
    DMA_InitBlock.DMA_PeripheralBaseAddr = (uint32_t)USART_DR_ADDRESS;
    // 内存地址(要传输的变量的指针)
    DMA_InitBlock.DMA_MemoryBaseAddr = (uint32_t)USART_RX_Buffer;
    // 方向：从内存到外设
    DMA_InitBlock.DMA_DIR = DMA_DIR_PeripheralSRC;
    // 传输大小
    DMA_InitBlock.DMA_BufferSize = USART_RECV_BUFFER_SIZE;
    // 外设地址不增
    DMA_InitBlock.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // 内存地址自增
    DMA_InitBlock.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 外设数据单位
    DMA_InitBlock.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    // 内存数据单位
    DMA_InitBlock.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    // DMA模式，一次或者循环模式
    // DMA_InitBlock.DMA_Mode = DMA_Mode_Normal ;
    DMA_InitBlock.DMA_Mode = DMA_Mode_Circular;
    // 优先级：中
    DMA_InitBlock.DMA_Priority = DMA_Priority_VeryHigh;
    // 禁止内存到内存的传输
    DMA_InitBlock.DMA_M2M = DMA_M2M_Disable;
    // 配置DMA通道
    DMA_Init(USART_RX_DMA_CHANNEL, &DMA_InitBlock);
    // 清除DMA所有标志
    DMA_ClearFlag(DMA1_FLAG_GL5);
    //DMA_ITConfig(USART_RX_DMA_CHANNEL, DMA_IT_TE, ENABLE);
    DMA_ITConfig(USART_RX_DMA_CHANNEL, DMA_IT_TC, ENABLE);
    // 使能DMA
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
// 中断处理器，单字符方式接收数据
void USART1_Recv_Handler ( void )
{
    char *p_mem_blk;


    if ( USART_GetITStatus ( USART1, USART_IT_RXNE ) != RESET )
    {
        *p_mem_blk = USART_ReceiveData ( USART1 );


    }
}
#else
// 中断处理器，DMA buffer方式接收数据
void USART1_Recv_Handler ( void )
{
    if ( USART_GetITStatus( USART1, USART_IT_IDLE ) != RESET )
    {
        DMA_Cmd(USART_RX_DMA_CHANNEL, DISABLE);

        USART_ReceiveData( USART1 );  /* 清除标志位 */

        // 清DMA标志位
        DMA_ClearITPendingBit( DMA1_IT_TC5 );
        // 重新赋值计数值，必须大于等于最大可能接收到的数据帧数目
        DMA_SetCurrDataCounter(USART_RX_DMA_CHANNEL, USART_RECV_BUFFER_SIZE);
        DMA_Cmd(USART_RX_DMA_CHANNEL, ENABLE);

    }
}
#endif

// 重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到USART1 */
    USART_SendData(USART1, (uint8_t) ch);

    /* 等待发送完毕 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;

    return (ch);
}

// 重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
    /* 等待串口1输入数据 */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) ;

    return (int)USART_ReceiveData(USART1);
}

/*********************************************END OF FILE**********************/

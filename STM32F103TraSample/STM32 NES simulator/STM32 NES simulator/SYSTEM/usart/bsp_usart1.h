#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f10x.h"
#include <stdio.h>


#define             USE_USART_DMA                          0

/**************************USART参数定义********************************/
#define             USART1_BAUD_RATE                       115200

//#define             USART1                                 USART1
#define             USART1_APBxClock_FUN                   RCC_APB2PeriphClockCmd
#define             USART1_CLK                             RCC_APB2Periph_USART1
#define             USART1_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             USART1_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             USART1_TX_PORT                         GPIOA   
#define             USART1_TX_PIN                          GPIO_Pin_9
#define             USART1_RX_PORT                         GPIOA 
#define             USART1_RX_PIN                          GPIO_Pin_10
#define             USART1_IRQ                             USART1_IRQn

// 串口对应的DMA请求通道
#define             USART_RX_DMA_CHANNEL                   DMA1_Channel5
// 外设寄存器地址
#define             USART_DR_ADDRESS                     (&USART1->DR)
// 一次发送的数据量
#define             USART_RECV_BUFFER_SIZE                 1024

void  USART1_Init     ( void );
void  USART1_Recv_Handler  ( void );


#endif /* __USART1_H__ */

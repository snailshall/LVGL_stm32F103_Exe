#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f10x.h"
#include <stdio.h>


#define             USE_USART_DMA                          0

/**************************USART��������********************************/
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

// ���ڶ�Ӧ��DMA����ͨ��
#define             USART_RX_DMA_CHANNEL                   DMA1_Channel5
// ����Ĵ�����ַ
#define             USART_DR_ADDRESS                     (&USART1->DR)
// һ�η��͵�������
#define             USART_RECV_BUFFER_SIZE                 1024

void  USART1_Init     ( void );
void  USART1_Recv_Handler  ( void );


#endif /* __USART1_H__ */

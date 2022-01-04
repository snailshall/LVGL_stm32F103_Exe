/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_tim.h"
#include "JoyPad.h"
#include "fc_key.h"

/*-------------------------------------------------------------------------------
a--------0x7f    01111111    127
b--------0xbf    10111111    191
select---0xdf    11011111    223
start----0xef    11101111    239
up-------0xf7    11110111    247
down-----0xfb    11111011    251
left-----0xfd    11111101    253
right----0xfe    11111110    254
-------------------------------------------------------------------------------*/
extern int FrameCounter;
extern JoyPadType JoyPad[2];
int SysTicks = 0;

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
//    u8 temp;
//    SysTicks++;
//    if(SysTicks == 60)
//    {
//        SysTicks = FrameCounter = 0;
//    }
//    temp = fc_read();
//    JoyPad[0].value  = temp;
//    JoyPad[0].value  = 1 << 20;
//    JoyPad[0].value |= !((temp >> 0) & 0x01) << 7;  //right
//    JoyPad[0].value |= !((temp >> 1) & 0x01) << 6;  //left
//    JoyPad[0].value |= !((temp >> 2) & 0x01) << 5;  //down
//    JoyPad[0].value |= !((temp >> 3) & 0x01) << 4;  //up
//    JoyPad[0].value |= !((temp >> 4) & 0x01) << 3;  //start
//    JoyPad[0].value |= !((temp >> 5) & 0x01) << 2;  //select
//    JoyPad[0].value |= !((temp >> 6) & 0x01) << 1;  //b
//    JoyPad[0].value |= !((temp >> 7) & 0x01) << 0;  //a
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        SysTicks++;
        if(SysTicks == 60)
        {
            SysTicks = FrameCounter = 0;
        }
        JoyPad[0].value = fc_read();
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

#include "timer.h"
#include "stm32f10x.h"
#include "lvgl.h"

//arr重装载值
//psc时钟分频数
//初始化定时器3
void TIM3_Config(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseBlock;
    NVIC_InitTypeDef NVIC_InitBlock;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM_TimeBaseBlock.TIM_Period = arr;
    TIM_TimeBaseBlock.TIM_Prescaler = psc;
    TIM_TimeBaseBlock.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseBlock.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseBlock);
    
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // [0:4]

    /* 配置中断源 */
    NVIC_InitBlock.NVIC_IRQChannel                    = TIM3_IRQn;
    NVIC_InitBlock.NVIC_IRQChannelPreemptionPriority  = 0;
    NVIC_InitBlock.NVIC_IRQChannelSubPriority         = 3;
    NVIC_InitBlock.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitBlock);
    
    TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        lv_tick_inc(1); // 1ms heartbeat
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

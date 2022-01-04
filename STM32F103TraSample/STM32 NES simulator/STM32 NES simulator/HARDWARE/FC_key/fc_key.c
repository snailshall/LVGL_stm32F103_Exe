#include "fc_key.h"
#include "delay.h"
#include "timer.h"

void delay(u32 nCount)
{
    volatile int i;
    for (i=0; i<nCount*100; i++);
}
//-------------------------------------------------------------------------------

void fc_init(void)
{
//    RCC->APB2ENR |= 1<<2;    // 使能PORT A时钟
//    GPIOA->CRH   &= 0XFFFFF000;
//    GPIOA->CRH   |= 0X00000383;
//    GPIOA->ODR   |= 1<<8|1<<9|1<<10;

    GPIO_InitTypeDef GPIO_InitBlock;

    RCC_APB2PeriphClockCmd ( KEY1_GPIO_CLK | KEY2_GPIO_CLK, ENABLE );

    GPIO_InitBlock.GPIO_Pin   = KEY1_GPIO_PIN;
    GPIO_InitBlock.GPIO_Mode  = KEY1_GPIO_Mode;
    GPIO_Init ( KEY1_GPIO_PORT, &GPIO_InitBlock );

    GPIO_InitBlock.GPIO_Pin   = KEY2_GPIO_PIN;
    GPIO_InitBlock.GPIO_Mode  = KEY2_GPIO_Mode;
    GPIO_Init ( KEY2_GPIO_PORT, &GPIO_InitBlock );

    TIM3_Config(167, 7199);

    *(volatile unsigned int*)0x40021018 |= ( 1 << 3 );
    // 配置端口为输出
    *(volatile unsigned int*)0x40010C00 |= ( 1 << (4*0) );
    // 控制ODR寄存器
    //*(volatile unsigned int*)0x40010C0C &= (unsigned int)( ~(1<<0) ); // LED ON
    *(volatile unsigned int*)0x40010C0C |= 1<<0; // LED OFF
}
//-------------------------------------------------------------------------------

u8 fc_read(void)
{
    u8 key = 0;

//    Clock=1;
//    Latch=1;
//    delay(5);
//    Latch=0;

    // 去抖动
//    if((right==1|left==1|down==1|up==1|start==1|select==1|b==1|a==1))
//    {
//        delay_ms(15);
//    }

//    if (right==1)       key=0;
//    else if (left==1)   key=1;
//    else if (down==1)   key=2;
//    else if (up==1)     key=3;
//    else if (start==1)  key=4;
//    else if (select==1) key=5;
//    else if (b==1)      key=6;
//    else if (a==1)      key=7;


    //key=0x01; // a
    //key=0x02; // b
    //key=0x04; // select
    //key=0x08; // start
    //key=0x10; // up
    //key=0x20; // down
    //key=0x40; // left
    //key=0x80; // right
    if(KEY1==1)
    {
        //key=0x20; //down
        key=0x08; //start
    }
    if(KEY2==1)
    {
        //key=0x04; // select
        key=0x80; // right
        //*(volatile unsigned int*)0x40010C0C &= (unsigned int)( ~(1<<0) ); // LED ON
    }
    //else
        //*(volatile unsigned int*)0x40010C0C |= 1<<0; // LED OFF

    return key;
}
//-------------------------------------------------------------------------------

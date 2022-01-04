#include "stm32f10x.h"

/************************************ ≈‰÷√ KEY1 *********************************/
#define               KEY1_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define               KEY1_GPIO_PORT                       GPIOA
#define               KEY1_GPIO_PIN                        GPIO_Pin_0
#define               KEY1_GPIO_Mode                       GPIO_Mode_IPD


/************************************ ≈‰÷√ KEY2 *********************************/
#define               KEY2_GPIO_CLK                        RCC_APB2Periph_GPIOC
#define               KEY2_GPIO_PORT                       GPIOC
#define               KEY2_GPIO_PIN                        GPIO_Pin_13
#define               KEY2_GPIO_Mode                       GPIO_Mode_IPD


#define KEY1  GPIO_ReadInputDataBit ( KEY1_GPIO_PORT, KEY1_GPIO_PIN )
#define KEY2  GPIO_ReadInputDataBit ( KEY2_GPIO_PORT, KEY2_GPIO_PIN )


u8 fc_read(void);
void fc_init(void);

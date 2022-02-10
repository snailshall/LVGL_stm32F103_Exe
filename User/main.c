/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��������ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"	
#include "./lcd/bsp_ili9341_lcd.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "./led/bsp_led.h" 
#include "palette.h"
#include <string.h>

#include "timer.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"


#define LV_USE_example1 	0x1u
#define LV_USE_example2	 	0x2u
#define LV_USE_example3 	0x4u
#define LV_USE_example4 	0x8u
#define LV_USE_example5 	0x10u
#define LV_USE_example6 	0x20u
#define LV_USE_example  	LV_USE_example4// ʹ��Touchpad 



#define LVGL_TICK 	5


#include "lv_demo_widgets.h"
#include "lv_example_anim.h"
#include "lv_example_widgets.h"
#include "lv_example_event.h"
#include "test1.h"
#include "lv_example_flex.h"
int main(void)
{
    TIM3_Config(999, 71);

    //LCD ��ʼ��
    ILI9341_Init();
    
    //��������ʼ��
    XPT2046_Init();
    //��FLASH���ȡУ����������FLASH�޲�������ʹ��ģʽ3����У��
    Calibrate_or_Get_TouchParaWithFlash(3, 0);
    
    /* USART config */
    USART_Config();
    LED_GPIO_Config();
    
    printf("\r\n ********** ����������� *********** \r\n"); 
    printf("\r\n ��������ʾ�����������Ķ������е�readme.txt�ļ�˵��������Ҫ���FLASH��ˢ��ģ����\r\n"); 
    
    //����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
    //���Ƽ�ʹ������ģʽ��ʾ���� ����ģʽ��ʾ���ֻ��о���Ч��
    //���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
    ILI9341_GramScan ( 3 );
    //ILI9341_DrawRectangle(100,100,20,20, 1); // test

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

 //   lv_demo_widgets(); 
	//	lv_example_animimg_1();

//#if ( LV_USE_example & LV_USE_example1 ) == LV_USE_example1
//			lv_example_anim_1();
//#endif

//#if ( LV_USE_example & LV_USE_example2 ) == LV_USE_example2
//			lv_example_anim_2();
//#endif

//#if ( LV_USE_example & LV_USE_example3 ) == LV_USE_example3
//			lv_example_anim_3();
//#endif
//#if ( LV_USE_example & LV_USE_example4 ) == LV_USE_example4
//			lv_example_anim_timeline_1();
//#endif



//#if ( LV_USE_example & LV_USE_example1 ) == LV_USE_example1
//			lv_example_event_1();
//#endif

//#if ( LV_USE_example & LV_USE_example2 ) == LV_USE_example2
//			lv_example_event_2();
//#endif

//#if ( LV_USE_example & LV_USE_example3 ) == LV_USE_example3
//			lv_example_event_3();
//#endif


//#if ( LV_USE_example & LV_USE_example1 ) == LV_USE_example1
//			lv_example_flex_1();
//#endif

//#if ( LV_USE_example & LV_USE_example2 ) == LV_USE_example2
//			lv_example_flex_2();
//#endif

//#if ( LV_USE_example & LV_USE_example3 ) == LV_USE_example3
//			lv_example_flex_3();
//#endif
//#if ( LV_USE_example & LV_USE_example4 ) == LV_USE_example4
//			lv_example_flex_4();
//#endif
//#if ( LV_USE_example & LV_USE_example5 ) == LV_USE_example6
//			lv_example_flex_5();
//#endif
//#if ( LV_USE_example & LV_USE_example6 ) == LV_USE_example6
//			lv_example_flex_6();
//#endif
//

//#if ( LV_USE_example & LV_USE_example1 ) == LV_USE_example1
//			lv_example_meter_1();
//#endif

//#if ( LV_USE_example & LV_USE_example2 ) == LV_USE_example2
//			lv_example_meter_2();
//#endif

//#if ( LV_USE_example & LV_USE_example3 ) == LV_USE_example3
//			lv_example_meter_3();
//#endif
//#if ( LV_USE_example & LV_USE_example4 ) == LV_USE_example4
//			lv_example_meter_4();
//#endif


//#if ( LV_USE_example & LV_USE_example1 ) == LV_USE_example1
//			lv_example_btn_1();
//#endif

//#if ( LV_USE_example & LV_USE_example2 ) == LV_USE_example2
//			lv_example_btn_2();
//#endif

//#if ( LV_USE_example & LV_USE_example3 ) == LV_USE_example3
//			lv_example_btn_3();
//#endif
//#if ( LV_USE_example & LV_USE_example4 ) == LV_USE_example4
//			lv_example_btnmatrix_1();
//#endif

//#if ( LV_USE_example & LV_USE_example5 ) == LV_USE_example5
//			lv_example_btnmatrix_2();
//#endif

//#if ( LV_USE_example & LV_USE_example6 ) == LV_USE_example6
//			lv_example_btnmatrix_3();
//#endif

lv_example_test();

//lv_example_win_1();
    //���ƴ����������
    //Palette_Init(LCD_SCAN_MODE);
    
    while ( 1 )
    {
        lv_task_handler();
			
				lv_tick_inc(LVGL_TICK);
				//lv_timer_handler();
				//HAL_Delay(LVGL_TICK);
        //������⺯��������������10ms����һ��
        //XPT2046_TouchEvenHandler();
    }
}


/* ------------------------------------------end of file---------------------------------------- */

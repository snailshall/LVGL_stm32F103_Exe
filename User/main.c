/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   触摸画板实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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
#define LV_USE_example  	LV_USE_example4// 使用Touchpad 



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

    //LCD 初始化
    ILI9341_Init();
    
    //触摸屏初始化
    XPT2046_Init();
    //从FLASH里获取校正参数，若FLASH无参数，则使用模式3进行校正
    Calibrate_or_Get_TouchParaWithFlash(3, 0);
    
    /* USART config */
    USART_Config();
    LED_GPIO_Config();
    
    printf("\r\n ********** 触摸画板程序 *********** \r\n"); 
    printf("\r\n 若汉字显示不正常，请阅读工程中的readme.txt文件说明，根据要求给FLASH重刷字模数据\r\n"); 
    
    //其中0、3、5、6 模式适合从左至右显示文字，
    //不推荐使用其它模式显示文字 其它模式显示文字会有镜像效果
    //其中 6 模式为大部分液晶例程的默认显示方向  
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
    //绘制触摸画板界面
    //Palette_Init(LCD_SCAN_MODE);
    
    while ( 1 )
    {
        lv_task_handler();
			
				lv_tick_inc(LVGL_TICK);
				//lv_timer_handler();
				//HAL_Delay(LVGL_TICK);
        //触摸检测函数，本函数至少10ms调用一次
        //XPT2046_TouchEvenHandler();
    }
}


/* ------------------------------------------end of file---------------------------------------- */

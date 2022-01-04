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
#include "porting/lv_port_disp.h"
#include "porting/lv_port_indev.h"

static void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, NULL);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

void lvgl_first_demo_start(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_set_event_cb(btn, btn_event_cb);                 /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
    lv_label_set_text(label, "Yeah");                       /*Set the labels text*/


    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label1, "Hello world!"); 
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(btn, label1, LV_ALIGN_OUT_TOP_MID, 0, -10);
}
#include "lv_demo_widgets/lv_demo_widgets.h"
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

    //lvgl_first_demo_start();
    lv_demo_widgets();
    
    //绘制触摸画板界面
    //Palette_Init(LCD_SCAN_MODE);
    
    while ( 1 )
    {
        lv_task_handler();
        //触摸检测函数，本函数至少10ms调用一次
        //XPT2046_TouchEvenHandler();
    }
}


/* ------------------------------------------end of file---------------------------------------- */

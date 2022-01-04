/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
#if LV_USE_GPU
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
        const lv_area_t * fill_area, lv_color_t color);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define RENDER_TYPE_NORMAL            1
// 一次准备指令，批量拷贝数据
#define RENDER_TYPE_NORMAL_OPTIMIZED  2
// 一次准备指令，DMA方式批量拷贝数据
#define RENDER_TYPE_DMA_OPTIMIZED     3
// 选择触摸屏的渲染方式
#define RENDER_TYPE                   RENDER_TYPE_DMA_OPTIMIZED
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /* LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed your display drivers `flush_cb` to copy its content to your dispay.
     * The buffer has to be greater than 1 display row
     *
     * There are three buffering configurations:
     * 1. Create ONE buffer with some rows: 
     *      LVGL will draw the display's content here and writes it to your display
     * 
     * 2. Create TWO buffer with some rows: 
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     * 
     * 3. Create TWO screen-sized buffer: 
     *      Similar to 2) but the buffer have to be screen sized. When LVGL is ready it will give the
     *      whole frame to display. This way you only need to change the frame buffer's address instead of
     *      copying the pixels.
     * */

    static lv_disp_buf_t draw_buf_dsc;

    /* Example for 1) */
    static lv_color_t draw_buf_1[LV_HOR_RES_MAX * 40];                          /*A buffer for 10 rows*/
    lv_disp_buf_init(&draw_buf_dsc, draw_buf_1, NULL, LV_HOR_RES_MAX * 40);   /*Initialize the display buffer*/

    /* Example for 2) */
    //static lv_color_t draw_buf_2_1[LV_HOR_RES_MAX * 10];                        /*A buffer for 10 rows*/
    //static lv_color_t draw_buf_2_2[LV_HOR_RES_MAX * 10];                        /*An other buffer for 10 rows*/
    //lv_disp_buf_init(&draw_buf_dsc, draw_buf_2_1, draw_buf_2_2, LV_HOR_RES_MAX * 10);   /*Initialize the display buffer*/

    /* Example for 3) */
    //static lv_color_t draw_buf_3_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*A screen sized buffer*/
    //static lv_color_t draw_buf_3_2[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*An other screen sized buffer*/
    //lv_disp_buf_init(&draw_buf_dsc, draw_buf_3_1, draw_buf_3_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);   /*Initialize the display buffer*/


    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX; //320;
    disp_drv.ver_res = LV_VER_RES_MAX; //240;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.buffer = &draw_buf_dsc;

#if LV_USE_GPU
    /*Optionally add functions to access the GPU. (Only in buffered mode, LV_VDB_SIZE != 0)*/

    /*Blend two color array using opacity*/
    disp_drv.gpu_blend_cb = gpu_blend;

    /*Fill a memory array with a color*/
    disp_drv.gpu_fill_cb = gpu_fill;
#endif

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#include "lcd/bsp_ili9341_lcd.h"

/* Initialize your display and the required peripherals. */
static void disp_init(void)
{
    /*You code here*/
}

#if (RENDER_TYPE == RENDER_TYPE_NORMAL_OPTIMIZED || RENDER_TYPE == RENDER_TYPE_DMA_OPTIMIZED)
#define LCD_WRITE_CMD(__cmd__)  (*(volatile uint16_t *)(FSMC_Addr_ILI9341_CMD)) = __cmd__;
#define LCD_WRITE_DATA(__data__)  (*(volatile uint16_t *)(FSMC_Addr_ILI9341_DATA)) = __data__;
#define LCD_RENDER_DRAW  LCD_WRITE_DATA

#define LCD_RENDER_READY(__x1__, __y1__, __x2__, __y2__)  \
{                                                         \
    LCD_WRITE_CMD ( CMD_SetCoordinateX );                 \
    LCD_WRITE_DATA ( ( __x1__ ) >> 8  );                  \
    LCD_WRITE_DATA ( ( __x1__ ) & 0xff  );                \
    LCD_WRITE_DATA ( ( __x2__ ) >> 8  );                  \
    LCD_WRITE_DATA ( ( __x2__ ) & 0xff  );                \
                                                          \
    LCD_WRITE_CMD ( CMD_SetCoordinateY );                 \
    LCD_WRITE_DATA ( ( __y1__ ) >> 8  );                  \
    LCD_WRITE_DATA ( ( __y1__ ) & 0xff  );                \
    LCD_WRITE_DATA ( ( __y2__ ) >> 8 );                   \
    LCD_WRITE_DATA ( ( __y2__ ) & 0xff );                 \
                                                          \
    LCD_WRITE_CMD ( CMD_SetPixel );                       \
}
#endif

#if (RENDER_TYPE == RENDER_TYPE_DMA_OPTIMIZED)
#include "stm32f10x.h"
#endif

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
#if (RENDER_TYPE == RENDER_TYPE_NORMAL)
    int32_t x;
    int32_t y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            /* Put a pixel to the display. For example: */
            /* put_px(x, y, *color_p)*/
            ILI9341_DrawPixel((uint16_t)x, (uint16_t)y, (uint16_t)color_p->full); // 效率极低，卡顿严重
            color_p++;
        }
    }
#endif

    /* 
     * 精简掉了每个像素传递前的准备指令。
     * 改为一次准备，批量传递像素数据，并改函数调用为宏定义替换
     * 测试效果比ILI9341_DrawPixel要好很多
     */
#if (RENDER_TYPE == RENDER_TYPE_NORMAL_OPTIMIZED)
    int32_t i, len;
    len = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
    LCD_RENDER_READY(area->x1, area->y1, area->x2, area->y2);
    for (i = 0; i < len; i++)
    {
        LCD_RENDER_DRAW(color_p->full);
        color_p++;
    }
#endif

    /* 
     * 精简掉了每个像素传递前的准备指令。
     * 改为一次准备，批量传递像素数据，并拷贝数据方式为DMA
     * 测试效果比批量传递数据要好一些，后续可以将死循环等待完成方式改为中断方式
     */
#if (RENDER_TYPE == RENDER_TYPE_DMA_OPTIMIZED)
    DMA_InitTypeDef DMA_InitBlock;

    LCD_RENDER_READY(area->x1, area->y1, area->x2, area->y2);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitBlock.DMA_PeripheralBaseAddr = FSMC_Addr_ILI9341_DATA;
    DMA_InitBlock.DMA_MemoryBaseAddr = (uint32_t)color_p;
    DMA_InitBlock.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitBlock.DMA_BufferSize = (uint32_t)((area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1));
    DMA_InitBlock.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitBlock.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitBlock.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitBlock.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitBlock.DMA_Mode = DMA_Mode_Normal;
    //DMA_InitBlock.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitBlock.DMA_Priority = DMA_Priority_High;
    DMA_InitBlock.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel5, &DMA_InitBlock);

    DMA_ClearFlag(DMA1_FLAG_GL5);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);

    while (DMA_GetITStatus(DMA1_IT_TC5) == RESET) ; // 等待DMA拷贝完成

    DMA_Cmd(DMA1_Channel5, DISABLE);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, DISABLE);
    DMA_ClearITPendingBit(DMA1_IT_GL5);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);
#endif

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}


/*OPTIONAL: GPU INTERFACE*/
#if LV_USE_GPU

/* If your MCU has hardware accelerator (GPU) then you can use it to blend to memories using opacity
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    /*It's an example code which should be done by your GPU*/
    uint32_t i;
    for(i = 0; i < length; i++) {
        dest[i] = lv_color_mix(dest[i], src[i], opa);
    }
}

/* If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
                    const lv_area_t * fill_area, lv_color_t color)
{
    /*It's an example code which should be done by your GPU*/
    int32_t x, y;
    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

    for(y = fill_area->y1; y <= fill_area->y2; y++) {
        for(x = fill_area->x1; x <= fill_area->x2; x++) {
            dest_buf[x] = color;
        }
        dest_buf+=dest_width;    /*Go to the next line*/
    }
}

#endif  /*LV_USE_GPU*/

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif

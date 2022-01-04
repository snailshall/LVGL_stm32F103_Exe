#include "delay.h"
//#include "sys.h"
//#include "usart.h"
#include "nes_main.h"
#include "fc_key.h"
#include "bsp_ili9341_lcd.h"

int main(void)
{
    //uart_init(115200);
    //USART1_Init();
    //delay_init();
    ILI9341_Init();
    //ILI9341_DrawRectangle(100, 100, 80, 40, 1); // test
    fc_init();
    NES_MainLoop();
    return 0;
}

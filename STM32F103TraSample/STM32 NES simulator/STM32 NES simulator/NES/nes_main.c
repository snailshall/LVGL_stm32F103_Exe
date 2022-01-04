#include "nes_main.h"

int FrameCounter;
//-------------------------------------------------------------------------------

/* NES 帧周期循环 */
void NES_FrameCycle(void)
{
    int clocks; //CPU执行时间
    FrameCounter = 0;
    for(;;)
    {
        /* scanline: 0~19 VBANK 段，若PPU使能NMI，将产生NMI 中断, */
        FrameCounter++;        // 帧计数器
        SpriteHitFlag = FALSE;
        for(PPU_scanline = 0; PPU_scanline < 20; PPU_scanline++)
        {
            exec6502(CLOCKS_PER_SCANLINE);
        }
        /* scanline: 20, PPU加载设置，执行一次空的扫描时间 */
        exec6502(CLOCKS_PER_SCANLINE);
        PPU_scanline++;   //20++
        PPU_Reg.R2 &= ~R2_SPR0_HIT;
        /* scanline: 21~261 */
        for(; PPU_scanline < SCAN_LINE_DISPALY_END_NUM; PPU_scanline++)
        {
            if((SpriteHitFlag == TRUE) && ((PPU_Reg.R2 & R2_SPR0_HIT) == 0))
            {
                clocks = sprite[0].x * CLOCKS_PER_SCANLINE / NES_DISP_WIDTH;
                exec6502(clocks);
                PPU_Reg.R2 |= R2_SPR0_HIT;
                exec6502(CLOCKS_PER_SCANLINE - clocks);
            }
            else
            {
                exec6502(CLOCKS_PER_SCANLINE);
            }
            if(PPU_Reg.R1 & (R1_BG_VISIBLE | R1_SPR_VISIBLE)) // 若为假，关闭显示
            {
                if(SpriteHitFlag == FALSE)
                {
                    NES_GetSpr0HitFlag(PPU_scanline - SCAN_LINE_DISPALY_START_NUM); // 查找Sprite #0 碰撞标志
                }
            }
            if(FrameCounter & 2) // 每2帧显示一次
            {
                NES_RenderLine(PPU_scanline - SCAN_LINE_DISPALY_START_NUM); // 水平同步与显示一行
            }
        }
        /* scanline: 262 完成一帧 */
        exec6502(CLOCKS_PER_SCANLINE);
        PPU_Reg.R2 |= R2_VBlank_Flag;   // 设置VBANK 标志
        /* 若使能PPU VBANK中断，则设置VBANK */
        if(PPU_Reg.R0 & R0_VB_NMI_EN)
        {
            NMI_Flag = SET1; // 完成一帧扫描，产生NMI中断
        }
    }
}
//-------------------------------------------------------------------------------

void NES_MainLoop(void)
{
    NesHeader *neshreader = (NesHeader *)rom_file;
    // 初始化6502存储器镜像
    init6502mem( 0,                    /* exp_rom */
                 0,                    /* sram 由卡类型决定, 暂不支持 */
                 (&rom_file[0x10]),    /* prg_rombank, 存储器大小 由卡类型决定 */
                 neshreader->romnum );
    reset6502();
    PPU_Init((&rom_file[0x10] + (neshreader->romnum * 0x4000)), (neshreader->romfeature & 0x01)); /* PPU_初始化 */
    NES_JoyPadInit();
    NES_LCD_Clear(0);
    NES_FrameCycle();
}
//-------------------------------------------------------------------------------

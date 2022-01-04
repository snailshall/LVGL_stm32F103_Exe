#include "nes_main.h"

int FrameCounter;
//-------------------------------------------------------------------------------

/* NES ֡����ѭ�� */
void NES_FrameCycle(void)
{
    int clocks; //CPUִ��ʱ��
    FrameCounter = 0;
    for(;;)
    {
        /* scanline: 0~19 VBANK �Σ���PPUʹ��NMI��������NMI �ж�, */
        FrameCounter++;        // ֡������
        SpriteHitFlag = FALSE;
        for(PPU_scanline = 0; PPU_scanline < 20; PPU_scanline++)
        {
            exec6502(CLOCKS_PER_SCANLINE);
        }
        /* scanline: 20, PPU�������ã�ִ��һ�οյ�ɨ��ʱ�� */
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
            if(PPU_Reg.R1 & (R1_BG_VISIBLE | R1_SPR_VISIBLE)) // ��Ϊ�٣��ر���ʾ
            {
                if(SpriteHitFlag == FALSE)
                {
                    NES_GetSpr0HitFlag(PPU_scanline - SCAN_LINE_DISPALY_START_NUM); // ����Sprite #0 ��ײ��־
                }
            }
            if(FrameCounter & 2) // ÿ2֡��ʾһ��
            {
                NES_RenderLine(PPU_scanline - SCAN_LINE_DISPALY_START_NUM); // ˮƽͬ������ʾһ��
            }
        }
        /* scanline: 262 ���һ֡ */
        exec6502(CLOCKS_PER_SCANLINE);
        PPU_Reg.R2 |= R2_VBlank_Flag;   // ����VBANK ��־
        /* ��ʹ��PPU VBANK�жϣ�������VBANK */
        if(PPU_Reg.R0 & R0_VB_NMI_EN)
        {
            NMI_Flag = SET1; // ���һ֡ɨ�裬����NMI�ж�
        }
    }
}
//-------------------------------------------------------------------------------

void NES_MainLoop(void)
{
    NesHeader *neshreader = (NesHeader *)rom_file;
    // ��ʼ��6502�洢������
    init6502mem( 0,                    /* exp_rom */
                 0,                    /* sram �ɿ����;���, �ݲ�֧�� */
                 (&rom_file[0x10]),    /* prg_rombank, �洢����С �ɿ����;��� */
                 neshreader->romnum );
    reset6502();
    PPU_Init((&rom_file[0x10] + (neshreader->romnum * 0x4000)), (neshreader->romfeature & 0x01)); /* PPU_��ʼ�� */
    NES_JoyPadInit();
    NES_LCD_Clear(0);
    NES_FrameCycle();
}
//-------------------------------------------------------------------------------

#include "nes_main.h"

JoyPadType JoyPad[2];

//-------------------------------------------------------------------------------
void NES_JoyPadInit(void)
{
    JoyPad[JOYPAD_0].state = JOYPADDISABLE;
    JoyPad[JOYPAD_0].index = 0;
    //JoyPad[JOYPAD_0].value = 1 << 20;
    JoyPad[JOYPAD_1].state = JOYPADDISABLE;
    JoyPad[JOYPAD_1].index = 0;
    //JoyPad[JOYPAD_1].value = 1 << 19;
}
//-------------------------------------------------------------------------------
void NES_JoyPadReset(void)
{
    JoyPad[JOYPAD_0].state = JOYPADENABLE;
    JoyPad[JOYPAD_0].index = 0;
    JoyPad[JOYPAD_1].state = JOYPADENABLE;
    JoyPad[JOYPAD_1].index = 0;
}
//-------------------------------------------------------------------------------
void NES_JoyPadDisable(void)
{
    JoyPad[JOYPAD_0].state = JOYPADDISABLE;
    JoyPad[JOYPAD_1].state = JOYPADDISABLE;
}
//-------------------------------------------------------------------------------
/*
 * 对应6502中0x4016地址的数据。
 * 从低向高依次读取JoyPad[JoyPadNum].value(uint32_t)中的每一个bit数据，读取一次自
 * 动向高位移动一个bit，最多读完32个bit。
 * JoyPad[JoyPadNum].value低8bit有效，bit=1为按下状态，bit=0为松开状态
 * bit0-7依次对应A, B, SELECT, START, UP, DOWN, LEFT, RIGHT

 * 经测试，返回的结果为0x40或0x41。
 * 0x40: 第n bit没有数据，即对应的key未按下
 * 0x41: 第n bit有数据，即对应的key被按下
 * 测试结果为n=[0-7]和n=[24-31]数据完全一样，n=[8-23]均为0x40
 */
uint8 NES_GetJoyPadVlaue(int JoyPadNum)
{
    uint8 retval = 0;

    // 以下是网上下载的源码，经测试，按键无效
    //retval = (JoyPad[JoyPadNum].value >> JoyPad[JoyPadNum].index) & 0x01; // A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
    //JoyPad[JoyPadNum].index++;

    // 经测试以下代码可以正常响应按键
    retval = ((JoyPad[JoyPadNum].value >> JoyPad[JoyPadNum].index) & 0x01) | 0x40;
    JoyPad[JoyPadNum].index = (JoyPad[JoyPadNum].index == 23) ? 0 : (JoyPad[JoyPadNum].index + 1);

    return retval;
}
//-------------------------------------------------------------------------------

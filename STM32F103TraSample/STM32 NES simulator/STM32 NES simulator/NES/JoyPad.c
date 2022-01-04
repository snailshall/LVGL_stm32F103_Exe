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
 * ��Ӧ6502��0x4016��ַ�����ݡ�
 * �ӵ�������ζ�ȡJoyPad[JoyPadNum].value(uint32_t)�е�ÿһ��bit���ݣ���ȡһ����
 * �����λ�ƶ�һ��bit��������32��bit��
 * JoyPad[JoyPadNum].value��8bit��Ч��bit=1Ϊ����״̬��bit=0Ϊ�ɿ�״̬
 * bit0-7���ζ�ӦA, B, SELECT, START, UP, DOWN, LEFT, RIGHT

 * �����ԣ����صĽ��Ϊ0x40��0x41��
 * 0x40: ��n bitû�����ݣ�����Ӧ��keyδ����
 * 0x41: ��n bit�����ݣ�����Ӧ��key������
 * ���Խ��Ϊn=[0-7]��n=[24-31]������ȫһ����n=[8-23]��Ϊ0x40
 */
uint8 NES_GetJoyPadVlaue(int JoyPadNum)
{
    uint8 retval = 0;

    // �������������ص�Դ�룬�����ԣ�������Ч
    //retval = (JoyPad[JoyPadNum].value >> JoyPad[JoyPadNum].index) & 0x01; // A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
    //JoyPad[JoyPadNum].index++;

    // ���������´������������Ӧ����
    retval = ((JoyPad[JoyPadNum].value >> JoyPad[JoyPadNum].index) & 0x01) | 0x40;
    JoyPad[JoyPadNum].index = (JoyPad[JoyPadNum].index == 23) ? 0 : (JoyPad[JoyPadNum].index + 1);

    return retval;
}
//-------------------------------------------------------------------------------

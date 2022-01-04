#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#include "nes_main.h"

#define JOYPAD_0    0
#define JOYPAD_1    1

typedef enum {JOYPADDISABLE = 0, JOYPADENABLE = !JOYPADDISABLE} JoyPadState;

typedef struct
{
    JoyPadState state;
    uint8       index;   //��ǰ��ȡλ
    uint32      value;   //JoyPad ��ǰֵ
} JoyPadType;

void  NES_JoyPadInit(void);
void  NES_JoyPadReset(void);
void  NES_JoyPadDisable(void);
uint8 NES_GetJoyPadVlaue(int JoyPadNum);

#endif

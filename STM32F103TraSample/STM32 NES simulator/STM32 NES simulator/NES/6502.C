/*
6502的CPU主频大约是1.78MHz，就是1秒钟能运行1.78M个CPU循环（cycles）。NES每秒生成60个帧，每个帧
有262条扫描线。那么每条扫描线的CPU循环数就是：1.78 M / 60 / 262 = 113.23cycles。由于现代的CPU主
频过高，我们在运行113个循环后可以睡眠一会。或者更简单的一个方法是，每一帧后，我们让CPU休息一会，
每一帧的循环数是 262 * 113
*/

#include "nes_main.h"

//6502标志位 flags = NVRBDIZC
#define C_FLAG      0x01        // 1: Carry
#define Z_FLAG      0x02        // 1: Zero
#define I_FLAG      0x04        // 1: Irq disabled
#define D_FLAG      0x08        // 1: Decimal mode flag (NES unused)
#define B_FLAG      0x10        // 1: Break
#define R_FLAG      0x20        // 1: Reserved (Always 1)
#define V_FLAG      0x40        // 1: Overflow
#define N_FLAG      0x80        // 1: Negative

/* flags = NVRBDIZC */
BYTE a_reg, x_reg, y_reg, stat_reg, sp_reg;
WORD pc_reg = 0;

/* 中断标志 */
CPU_InitFlag NMI_Flag;
CPU_InitFlag IRQ_Flag;

/* Macros for convenience */
/*
累加寄存器A(这是8位的寄存器，也是6502中最重要和最常用的寄存器，可用于读写数据，进行各种逻辑运算等等。)
*/
#define A a_reg
/*
变址寄存器X(这个8位寄存器和A差不多，只是它可以非常方便地加1或减1，常用于数据传送，运算等等)
*/
#define X x_reg
/*
变址寄存器Y(和X寄存器一样，方便搭配使用(比如用作表示背景卷轴的X,Y坐标))
*/
#define Y y_reg
/*
状态寄存器[也叫程序状态字PSW(Program Status Word)]
这是8位寄存器，用于寄存指令执行的状态信息，PSW的各位状态有的是根据指令执行的结果由硬件自动设置的，有的可以由用户用指令设定。
PSW的各位意义如下(其中第5位E为扩展位，供基于6502扩展的处理器使用，保持为0，但不确定)
7 6 5 4 3 2 1 0
N V E B D I Z C
N:负数标志，指令指行完后为负(>7F)则为1,否则为0
V:溢出标志，若产生溢出则V=1,否则V=0
B:Break指令，用于判别中断是由BRK指令引起还是实际的中断引起
D:十进制模式标志，为1表示十进制模式，为0表示16进制模式。(一旦进入十进制模式ADC和SBC两条指令会把操作数当作10进制数，不推荐使用)
I:中断禁用标志，是否允许系统中断IRQ，=1:禁止，=0:允许
Z:零标志，结果是否为0,为0则Z=1,否则Z=0
C:进位标志,结果最高位有进位则C=1,否则C=0
*/
#define P stat_reg
/*
堆栈指针寄存器SP(Stack Pointer)
这是8位的寄存器，指向栈顶位置
在程序运行时。需要一个称为堆栈的RAM块作为数据缓冲区，以暂存程序运行过程中的一些重要数据
堆栈由连续的RAM单元组成。数据写入堆栈称入栈，数据从堆栈中读出称为出栈，堆栈的操作遵循"先进后出"的操作。
系统如何了解当前的堆栈顶在哪以从顶部弹出堆栈呢？就要依靠堆栈指针SP。
6502CPU规范中，堆栈位于第1个页面($0100-$01ff)的RAM中，因此SP指向堆栈顶的低8位，即堆栈顶地址为$00SP。
当进行入栈、出栈操作时，栈顶单元发生变化，SP的内容也将加1或减1
*/
#define SP sp_reg
/*
程序计数器PC(Program Counter)
这是6502唯一的一个16位计数器，其内容为将要执行命令的存放地址，寻址范围达64KB($0000-$FFFF)，CPU是根据PC的
内容找到指令的存储单元并取出指令执行的。PC具有自动加1的功能，CPU每读取一条指令的1字节，PC内容自动加1，指
向下一个存储单元，这样就能实现程序的顺序执行。
用户不能对PC进行读写，但可以通过跳转，调用，返回等指令改变其内容，以改变程序的执行顺序
*/
#define PC pc_reg

/* internal registers */
BYTE opcode;
int  clockticks6502;

/* help variables */
WORD savepc;
BYTE value;
int  sum, saveflags;
WORD help;

/* 6502 memory map */
uint8  ram6502[0x800];  /* RAM */
//uint8* ppu_regbase;     /* PPU IO reg */
//uint8* apu_regbase;     /* APU IO reg */
uint8* exp_rom;         /* expansion rom */
uint8* sram;            /* sram */
uint8* prg_rombank0;    /* prg-rom lower bank */
uint8* prg_rombank1;    /* prg-rom upper bank */

/* arrays */
typedef struct
{
    uint16 ticks;
    void (*instruction)(void);
    void (*addrmode)(void);
} OPCODE;

const OPCODE opcodetable[] =
{
    /*0x00*/    {7, brk6502,    implied6502},
    /*0x01*/    {6, ora6502,    indx6502},
    /*0x02*/    {2, nop6502,    implied6502},
    /*0x03*/    {2, nop6502,    implied6502},
    /*0x04*/    {3, tsb6502,    zp6502},
    /*0x05*/    {3, ora6502,    zp6502},
    /*0x06*/    {5, asl6502,    zp6502},
    /*0x07*/    {2, nop6502,    implied6502},
    /*0x08*/    {3, php6502,    implied6502},
    /*0x09*/    {3, ora6502,    immediate6502},
    /*0x0A*/    {2, asla6502,   implied6502},
    /*0x0B*/    {2, ora6502,    implied6502},
    /*0x0C*/    {4, tsb6502,    abs6502},
    /*0x0D*/    {4, ora6502,    abs6502},
    /*0x0E*/    {6, asl6502,    abs6502},
    /*0x0F*/    {2, nop6502,    implied6502},
    /*0x10*/    {2, bpl6502,    relative6502},
    /*0x11*/    {5, ora6502,    indy6502},
    /*0x12*/    {3, ora6502,    indzp6502},
    /*0x13*/    {2, nop6502,    implied6502},
    /*0x14*/    {3, trb6502,    zp6502},
    /*0x15*/    {4, ora6502,    zpx6502},
    /*0x16*/    {6, asl6502,    zpx6502},
    /*0x17*/    {2, nop6502,    implied6502},
    /*0x18*/    {2, clc6502,    implied6502},
    /*0x19*/    {4, ora6502,    absy6502},
    /*0x1A*/    {2, ina6502,    implied6502},
    /*0x1B*/    {2, nop6502,    implied6502},
    /*0x1C*/    {4, trb6502,    abs6502},
    /*0x1D*/    {4, ora6502,    absx6502},
    /*0x1E*/    {7, asl6502,    absx6502},
    /*0x1F*/    {2, nop6502,    implied6502},
    /*0x20*/    {6, jsr6502,    abs6502},
    /*0x21*/    {6, and6502,    indx6502},
    /*0x22*/    {2, nop6502,    implied6502},
    /*0x23*/    {2, nop6502,    implied6502},
    /*0x24*/    {3, bit6502,    zp6502},
    /*0x25*/    {3, and6502,    zp6502},
    /*0x26*/    {5, rol6502,    zp6502},
    /*0x27*/    {2, nop6502,    implied6502},
    /*0x28*/    {4, plp6502,    implied6502},
    /*0x29*/    {3, and6502,    immediate6502},
    /*0x2A*/    {2, rola6502,   implied6502},
    /*0x2B*/    {2, nop6502,    implied6502},
    /*0x2C*/    {4, bit6502,    abs6502},
    /*0x2D*/    {4, and6502,    abs6502},
    /*0x2E*/    {6, rol6502,    abs6502},
    /*0x2F*/    {2, nop6502,    implied6502},
    /*0x30*/    {2, bmi6502,    relative6502},
    /*0x31*/    {5, and6502,    indy6502},
    /*0x32*/    {3, and6502,    indzp6502},
    /*0x33*/    {2, nop6502,    implied6502},
    /*0x34*/    {4, bit6502,    zpx6502},
    /*0x35*/    {4, and6502,    zpx6502},
    /*0x36*/    {6, rol6502,    zpx6502},
    /*0x37*/    {2, nop6502,    implied6502},
    /*0x38*/    {2, sec6502,    implied6502},
    /*0x39*/    {4, and6502,    absy6502},
    /*0x3A*/    {2, dea6502,    implied6502},
    /*0x3B*/    {2, nop6502,    implied6502},
    /*0x3C*/    {4, bit6502,    absx6502},
    /*0x3D*/    {4, and6502,    absx6502},
    /*0x3E*/    {7, rol6502,    absx6502},
    /*0x3F*/    {2, nop6502,    implied6502},
    /*0x40*/    {6, rti6502,    implied6502},
    /*0x41*/    {6, eor6502,    indx6502},
    /*0x42*/    {2, nop6502,    implied6502},
    /*0x43*/    {2, nop6502,    implied6502},
    /*0x44*/    {2, nop6502,    implied6502},
    /*0x45*/    {3, eor6502,    zp6502},
    /*0x46*/    {5, lsr6502,    zp6502},
    /*0x47*/    {2, nop6502,    implied6502},
    /*0x48*/    {3, pha6502,    implied6502},
    /*0x49*/    {3, eor6502,    immediate6502},
    /*0x4A*/    {2, lsra6502,   implied6502},
    /*0x4B*/    {2, nop6502,    implied6502},
    /*0x4C*/    {3, jmp6502,    abs6502},
    /*0x4D*/    {4, eor6502,    abs6502},
    /*0x4E*/    {6, lsr6502,    abs6502},
    /*0x4F*/    {2, nop6502,    implied6502},
    /*0x50*/    {2, bvc6502,    relative6502},
    /*0x51*/    {5, eor6502,    indy6502},
    /*0x52*/    {3, eor6502,    indzp6502},
    /*0x53*/    {2, nop6502,    implied6502},
    /*0x54*/    {2, nop6502,    implied6502},
    /*0x55*/    {4, eor6502,    zpx6502},
    /*0x56*/    {6, lsr6502,    zpx6502},
    /*0x57*/    {2, nop6502,    implied6502},
    /*0x58*/    {2, cli6502,    implied6502},
    /*0x59*/    {4, eor6502,    absy6502},
    /*0x5A*/    {3, phy6502,    implied6502},
    /*0x5B*/    {2, nop6502,    implied6502},
    /*0x5C*/    {2, nop6502,    implied6502},
    /*0x5D*/    {4, eor6502,    absx6502},
    /*0x5E*/    {7, lsr6502,    absx6502},
    /*0x5F*/    {2, nop6502,    implied6502},
    /*0x60*/    {6, rts6502,    implied6502},
    /*0x61*/    {6, adc6502,    indx6502},
    /*0x62*/    {2, nop6502,    implied6502},
    /*0x63*/    {2, nop6502,    implied6502},
    /*0x64*/    {3, stz6502,    zp6502},
    /*0x65*/    {3, adc6502,    zp6502},
    /*0x66*/    {5, ror6502,    zp6502},
    /*0x67*/    {2, nop6502,    implied6502},
    /*0x68*/    {4, pla6502,    implied6502},
    /*0x69*/    {3, adc6502,    immediate6502},
    /*0x6A*/    {2, rora6502,   implied6502},
    /*0x6B*/    {2, nop6502,    implied6502},
    /*0x6C*/    {5, jmp6502,    indirect6502},
    /*0x6D*/    {4, adc6502,    abs6502},
    /*0x6E*/    {6, ror6502,    abs6502},
    /*0x6F*/    {2, nop6502,    implied6502},
    /*0x70*/    {2, bvs6502,    implied6502},
    /*0x71*/    {5, adc6502,    indy6502},
    /*0x72*/    {3, adc6502,    indzp6502},
    /*0x73*/    {2, nop6502,    implied6502},
    /*0x74*/    {4, stz6502,    zpx6502},
    /*0x75*/    {4, adc6502,    zpx6502},
    /*0x76*/    {6, ror6502,    zpx6502},
    /*0x77*/    {2, nop6502,    implied6502},
    /*0x78*/    {2, sei6502,    implied6502},
    /*0x79*/    {4, adc6502,    absy6502},
    /*0x7A*/    {4, ply6502,    implied6502},
    /*0x7B*/    {2, nop6502,    implied6502},
    /*0x7C*/    {6, jmp6502,    indabsx6502},
    /*0x7D*/    {4, adc6502,    absx6502},
    /*0x7E*/    {7, ror6502,    absx6502},
    /*0x7F*/    {2, nop6502,    implied6502},
    /*0x80*/    {2, bra6502,    relative6502},
    /*0x81*/    {6, sta6502,    indx6502},
    /*0x82*/    {2, nop6502,    implied6502},
    /*0x83*/    {2, nop6502,    implied6502},
    /*0x84*/    {2, sty6502,    zp6502},
    /*0x85*/    {2, sta6502,    zp6502},
    /*0x86*/    {2, stx6502,    zp6502},
    /*0x87*/    {2, nop6502,    implied6502},
    /*0x88*/    {2, dey6502,    implied6502},
    /*0x89*/    {2, bit6502,    immediate6502},
    /*0x8A*/    {2, txa6502,    implied6502},
    /*0x8B*/    {2, nop6502,    implied6502},
    /*0x8C*/    {4, sty6502,    abs6502},
    /*0x8D*/    {4, sta6502,    abs6502},
    /*0x8E*/    {4, stx6502,    abs6502},
    /*0x8F*/    {2, nop6502,    implied6502},
    /*0x90*/    {2, bcc6502,    relative6502},
    /*0x91*/    {6, sta6502,    indy6502},
    /*0x92*/    {3, sta6502,    indzp6502},
    /*0x93*/    {2, nop6502,    implied6502},
    /*0x94*/    {4, sty6502,    zpx6502},
    /*0x95*/    {4, sta6502,    zpx6502},
    /*0x96*/    {4, stx6502,    zpy6502},
    /*0x97*/    {2, nop6502,    implied6502},
    /*0x98*/    {2, tya6502,    implied6502},
    /*0x99*/    {5, sta6502,    absy6502},
    /*0x9A*/    {2, txs6502,    implied6502},
    /*0x9B*/    {2, nop6502,    implied6502},
    /*0x9C*/    {4, stz6502,    abs6502},
    /*0x9D*/    {5, sta6502,    absx6502},
    /*0x9E*/    {5, stz6502,    absx6502},
    /*0x9F*/    {2, nop6502,    implied6502},
    /*0xA0*/    {3, ldy6502,    immediate6502},
    /*0xA1*/    {6, lda6502,    indx6502},
    /*0xA2*/    {3, ldx6502,    immediate6502},
    /*0xA3*/    {2, nop6502,    implied6502},
    /*0xA4*/    {3, ldy6502,    zp6502},
    /*0xA5*/    {3, lda6502,    zp6502},
    /*0xA6*/    {3, ldx6502,    zp6502},
    /*0xA7*/    {2, nop6502,    implied6502},
    /*0xA8*/    {2, tay6502,    implied6502},
    /*0xA9*/    {3, lda6502,    immediate6502},
    /*0xAA*/    {2, tax6502,    implied6502},
    /*0xAB*/    {2, nop6502,    implied6502},
    /*0xAC*/    {4, ldy6502,    abs6502},
    /*0xAD*/    {4, lda6502,    abs6502},
    /*0xAE*/    {4, ldx6502,    abs6502},
    /*0xAF*/    {2, nop6502,    implied6502},
    /*0xB0*/    {2, bcs6502,    relative6502},
    /*0xB1*/    {5, lda6502,    indy6502},
    /*0xB2*/    {3, lda6502,    indzp6502},
    /*0xB3*/    {2, nop6502,    implied6502},
    /*0xB4*/    {4, ldy6502,    zpx6502},
    /*0xB5*/    {4, lda6502,    zpx6502},
    /*0xB6*/    {4, ldx6502,    zpy6502},
    /*0xB7*/    {2, nop6502,    implied6502},
    /*0xB8*/    {2, clv6502,    implied6502},
    /*0xB9*/    {4, lda6502,    absy6502},
    /*0xBA*/    {2, tsx6502,    implied6502},
    /*0xBB*/    {2, nop6502,    implied6502},
    /*0xBC*/    {4, ldy6502,    absx6502},
    /*0xBD*/    {4, lda6502,    absx6502},
    /*0xBE*/    {4, ldx6502,    absy6502},
    /*0xBF*/    {2, nop6502,    implied6502},
    /*0xC0*/    {3, cpy6502,    immediate6502},
    /*0xC1*/    {6, cmp6502,    indx6502},
    /*0xC2*/    {2, nop6502,    implied6502},
    /*0xC3*/    {2, nop6502,    implied6502},
    /*0xC4*/    {3, cpy6502,    zp6502},
    /*0xC5*/    {3, cmp6502,    zp6502},
    /*0xC6*/    {5, dec6502,    zp6502},
    /*0xC7*/    {2, nop6502,    implied6502},
    /*0xC8*/    {2, iny6502,    implied6502},
    /*0xC9*/    {3, cmp6502,    immediate6502},
    /*0xCA*/    {2, dex6502,    implied6502},
    /*0xCB*/    {2, nop6502,    implied6502},
    /*0xCC*/    {4, cpy6502,    abs6502},
    /*0xCD*/    {4, cmp6502,    abs6502},
    /*0xCE*/    {6, dec6502,    abs6502},
    /*0xCF*/    {2, nop6502,    implied6502},
    /*0xD0*/    {2, bne6502,    relative6502},
    /*0xD1*/    {5, cmp6502,    indy6502},
    /*0xD2*/    {3, cmp6502,    indzp6502},
    /*0xD3*/    {2, nop6502,    implied6502},
    /*0xD4*/    {2, nop6502,    implied6502},
    /*0xD5*/    {4, cmp6502,    zpx6502},
    /*0xD6*/    {6, dec6502,    zpx6502},
    /*0xD7*/    {2, nop6502,    implied6502},
    /*0xD8*/    {2, cld6502,    implied6502},
    /*0xD9*/    {4, cmp6502,    absy6502},
    /*0xDA*/    {3, phx6502,    implied6502},
    /*0xDB*/    {2, nop6502,    implied6502},
    /*0xDC*/    {2, nop6502,    implied6502},
    /*0xDD*/    {4, cmp6502,    absx6502},
    /*0xDE*/    {7, dec6502,    absx6502},
    /*0xDF*/    {2, nop6502,    implied6502},
    /*0xE0*/    {3, cpx6502,    immediate6502},
    /*0xE1*/    {6, sbc6502,    indx6502},
    /*0xE2*/    {2, nop6502,    implied6502},
    /*0xE3*/    {2, nop6502,    implied6502},
    /*0xE4*/    {3, cpx6502,    zp6502},
    /*0xE5*/    {3, sbc6502,    zp6502},
    /*0xE6*/    {5, inc6502,    zp6502},
    /*0xE7*/    {2, nop6502,    implied6502},
    /*0xE8*/    {2, inx6502,    implied6502},
    /*0xE9*/    {3, sbc6502,    immediate6502},
    /*0xEA*/    {2, nop6502,    implied6502},
    /*0xEB*/    {2, nop6502,    implied6502},
    /*0xEC*/    {4, cpx6502,    abs6502},
    /*0xED*/    {4, sbc6502,    abs6502},
    /*0xEE*/    {6, inc6502,    abs6502},
    /*0xEF*/    {2, nop6502,    implied6502},
    /*0xF0*/    {2, beq6502,    relative6502},
    /*0xF1*/    {5, sbc6502,    indy6502},
    /*0xF2*/    {3, sbc6502,    indzp6502},
    /*0xF3*/    {2, nop6502,    implied6502},
    /*0xF4*/    {2, nop6502,    implied6502},
    /*0xF5*/    {4, sbc6502,    zpx6502},
    /*0xF6*/    {6, inc6502,    zpx6502},
    /*0xF7*/    {2, nop6502,    implied6502},
    /*0xF8*/    {2, sed6502,    implied6502},
    /*0xF9*/    {4, sbc6502,    absy6502},
    /*0xFA*/    {4, plx6502,    implied6502},
    /*0xFB*/    {2, nop6502,    implied6502},
    /*0xFC*/    {2, nop6502,    implied6502},
    /*0xFD*/    {4, sbc6502,    absx6502},
    /*0xFE*/    {7, inc6502,    absx6502},
    /*0xFF*/    {2, nop6502,    implied6502}
};

//-------------------------------------------------------------------------------
void SprDMA(BYTE scr_addr)      //scr_addr 为高8位地址
{
    uint8 *scr_addrptr = 0;     //将传送sprite数据 指针
    int i;

    switch(scr_addr >> 4)       //选择源地址数据区域
    {
    case 0x0: //RAM
    case 0x1:
        scr_addrptr = &ram6502[(scr_addr << 8)];
        break;
    case 0x6: //SRAM
    case 0x7:
        scr_addrptr = sram + ((scr_addr << 8) - 0x6000);
        break;
    case 0x8: /*程序存储器 只读*/
    case 0x9:
    case 0xA:
    case 0xB:
        scr_addrptr = &prg_rombank0[(scr_addr << 8) - 0x8000];
        break;
    case 0xC:
    case 0xD:
    case 0xE:
    case 0xF:
        scr_addrptr = &prg_rombank1[(scr_addr << 8) - 0xC000];
        break;
    }

    for(i=0; i<256; i++)
    {
        Spr_Mem.spr_ram[i] = scr_addrptr[i];
    }

    //每次传输，需要512 cycles (about 4.5 scanlines worth)
    clockticks6502 -= 512; //花掉512CPU时钟周期
}

//-------------------------------------------------------------------------------
//读取6502存储器
int get6502memory(WORD addr) //没0x2000递增
{
    switch(addr & 0xE000)
    {
    case 0x0000:    //$0000 ~ $0FFF
        return (ram6502[addr&0x7FF]);
    case 0x1000:    //$1000 ~ $1FFF 镜像
        return (ram6502[addr&0x7FF]);
    case 0x2000:    //$2000 ~ $2FFF
        return PPU_RegRead(addr&0x7);  /*PPU IO接口*/
    case 0x3000:    //$3000 ~ $3FFF 镜像
        break;
    case 0x4000:    //$4000 ~ $4FFF
        /*APU 声音处理 IO接口 扩展ROM*/
        if(addr < 0x4013)
        {
            //return (apu_regbase[addr&0x1F]);  //暂不处理
        }
        if(addr == 0x4014)
        {
            //SpriteRAM_DMA(); break; 只支持写
        }
        if(addr == 0x4015)  //pAPU Sound / Vertical Clock Signal Register
        {
        }
        if(addr == 0x4016)
        {
            return NES_GetJoyPadVlaue(JOYPAD_0);//分为3段8bit数据，第一段8bit为控制器1,第二段控制器3，第三段ID code
        }
        if(addr == 0x4017)
        {
            return NES_GetJoyPadVlaue(JOYPAD_1);//分为3段8bit数据，第一段8bit为控制器2,第二段控制器4，第三段ID code
        }
        break;
    case 0x6000:    /*卡带SRAM，一般用来存储游戏进度*/
        //暂不处理
        break;
    case 0x8000:    /*程序存储器 只读 */
    case 0xA000:
        return prg_rombank0[addr-0x8000];
    case 0xC000:
    case 0xE000:
        return prg_rombank1[addr-0xC000];
    }
    return (addr >> 8); /*通常不会执行到此处,返回地址高八位
                          APU和SRAM未完成，可能执行到此处*/
}

//-------------------------------------------------------------------------------
/* 6502存储器写入 */
void put6502memory(WORD addr, BYTE value)
{
    switch(addr & 0xE000)
    {
    case 0x0000:
        ram6502[addr&0x7FF] = value;
        break;
    case 0x2000:    /*PPU IO接口*/
        PPU_RegWrite(addr&0x7, value);
        break;
    case 0x4000:    /*APU 声音处理 IO接口 扩展ROM*/
        if(addr == 0x4014)
        {
            SprDMA(value);     //value 为将要传送的数据地址的高八位
        }
        if(addr == 0x4016)
        {
            /*bit0 JoyPad 控制*/
            if(value & 1)      //先写bit0:1复位按键状态，在写bit0:0禁止
            {
                NES_JoyPadReset();
            }
            else
            {
                NES_JoyPadDisable();
            }
            /*其他位暂未使用*/
        }
        break;
    case 0x6000:    /*卡带SRAM，一般用来存储游戏进度*/
        break;
    case 0x8000:    /*程序存储器 只读(写入时与存储器切换(mapper)有关，暂略)*/
    case 0xA000:
    case 0xC000:
    case 0xE000:
        break;
    }
}

//-------------------------------------------------------------------------------
/* 6502 寻址模式 */
/* Addressing modes */
/* Implied */
void implied6502(void)
{
}

//-------------------------------------------------------------------------------
/* #Immediate */
void immediate6502(void)
{
    savepc = PC++;
}

//-------------------------------------------------------------------------------
/* ABS */
void abs6502(void)
{
    savepc  =  get6502memory(PC);
    PC++;
    savepc += (get6502memory(PC) << 8);
    PC++;
}

//-------------------------------------------------------------------------------
/* Branch */
void relative6502(void)
{
    savepc = get6502memory(PC);
    PC++;
    if (savepc & 0x80)
    {
        savepc -= 0x100;
    }
    if ((savepc>>8) != (PC>>8))
    {
        clockticks6502++;
    }
}

//-------------------------------------------------------------------------------
/* (ABS) */
void indirect6502(void)
{
    help    =  get6502memory(PC);
    PC++;
    help   += (get6502memory(PC) << 8);
    PC++;
    savepc  =  get6502memory(help);
    savepc += (get6502memory(help+1) << 8);
}

//-------------------------------------------------------------------------------
/* ABS,X */
void absx6502(void)
{
    savepc  =  get6502memory(PC);
    PC++;
    savepc += (get6502memory(PC) << 8);
    PC++;
    if (opcodetable[opcode].ticks == 4)
    {
        if ((savepc>>8) != ((savepc+X)>>8))
        {
            clockticks6502++;
        }
    }
    savepc += X;
}

//-------------------------------------------------------------------------------
/* ABS,Y */
void absy6502(void)
{
    savepc  =  get6502memory(PC);
    PC++;
    savepc += (get6502memory(PC) << 8);
    PC++;
    if (opcodetable[opcode].ticks == 4)
    {
        if ((savepc>>8) != ((savepc+Y)>>8))
        {
            clockticks6502++;
        }
    }
    savepc += Y;
}

//-------------------------------------------------------------------------------
/* ZP */
void zp6502(void)
{
    savepc  = get6502memory(PC);
    PC++;
}

//-------------------------------------------------------------------------------
/* ZP,X */
void zpx6502(void)
{
    savepc  = get6502memory(PC) + X;
    PC++;
    savepc &= 0x00ff;
}

//-------------------------------------------------------------------------------
/* ZP,Y */
void zpy6502(void)
{
    savepc  = get6502memory(PC) + Y;
    PC++;
    savepc &= 0x00ff;
}

//-------------------------------------------------------------------------------
/* (ZP,X) */
void indx6502(void)
{
    value   = get6502memory(PC) + Y;
    PC++;
    savepc  = get6502memory(value);
    savepc += get6502memory(value + 1) << 8;
}

//-------------------------------------------------------------------------------
/* (ZP),Y */
void indy6502(void)
{
    value   = get6502memory(PC);
    PC++;
    savepc  = get6502memory(value);
    savepc += get6502memory(value + 1) << 8;
    if (opcodetable[opcode].ticks == 5)
    {
        if ((savepc>>8) != ((savepc+Y)>>8))
        {
            clockticks6502++;
        }
    }
    savepc += Y;
}

//-------------------------------------------------------------------------------
/* (ABS,X) */
void indabsx6502(void)
{
    help    =  get6502memory(PC);
    help    = (get6502memory(PC + 1) << 8) + X;
    savepc  =  get6502memory(help);
    savepc +=  get6502memory(help + 1) << 8;
}

//-------------------------------------------------------------------------------
/* (ZP) */
void indzp6502(void)
{
    value   = get6502memory(PC);
    PC++;
    savepc  = get6502memory(value);
    savepc += get6502memory(value + 1) << 8;
}

//-------------------------------------------------------------------------------
// 6502 指令
/* Instructions */
void adc6502(void)
{
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    saveflags = (P & 0x01);
    sum = ((char)A) + ((char)value) + saveflags;
    if ((sum>0x7f) || (sum<-0x80))
    {
        P |= 0x40;
    }
    else
    {
        P &= 0xbf;
    }
    sum = A + value + saveflags;
    if (sum>0xff)
    {
        P |= 0x01;
    }
    else
    {
        P &= 0xfe;
    }
    A = sum;
    if (P & 0x08)
    {
        P &= 0xfe;
        if ((A & 0x0f)>0x09)
        {
            A += 0x06;
        }
        if ((A & 0xf0)>0x90)
        {
            A += 0x60;
            P |= 0x01;
        }
    }
    else
    {
        clockticks6502++;
    }
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void and6502(void)
{
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    A &= value;
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void asl6502(void)
{
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    P = (P & 0xfe) | ((value >>7) & 0x01);
    value = value << 1;
    put6502memory(savepc,value);
    if (value)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (value & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void asla6502(void)
{
    P = (P & 0xfe) | ((A >>7) & 0x01);
    A = A << 1;
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void bcc6502(void)
{
    if ((P & 0x01) == 0)
    {
        opcodetable[opcode].addrmode();
        PC += savepc;
        clockticks6502++;
    }
    else
    {
        value = get6502memory(PC);
        PC++;
    }
}

//-------------------------------------------------------------------------------
void bcs6502(void)
{
    if (P & 0x01)
    {
        opcodetable[opcode].addrmode();
        PC += savepc;
        clockticks6502++;
    }
    else
    {
        value = get6502memory(PC);
        PC++;
    }
}

//-------------------------------------------------------------------------------
void beq6502(void)
{
    if (P & 0x02)
    {
        opcodetable[opcode].addrmode();
        PC += savepc;
        clockticks6502++;
    }
    else
    {
        value = get6502memory(PC);
        PC++;
    }
}

//-------------------------------------------------------------------------------
void bit6502(void)
{
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    /* non-destrucive logically And between value and the accumulator
    * and set zero flag */
    if (value & A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    /* set negative and overflow flags from value */
    P = (P & 0x3f) | (value & 0xc0);
}

//-------------------------------------------------------------------------------
void bmi6502(void)
{
    if (P & 0x80)
    {
        opcodetable[opcode].addrmode();
        PC += savepc;
        clockticks6502++;
    }
    else
    {
        value = get6502memory(PC);
        PC++;
    }
}

//-------------------------------------------------------------------------------
void bne6502(void)
{
    if ((P & 0x02)==0)
    {
        opcodetable[opcode].addrmode();
        PC += savepc;
        clockticks6502++;
    }
    else
    {
        value = get6502memory(PC);
        PC++;
    }
}

//-------------------------------------------------------------------------------
void bpl6502(void)
{
    if ((P & 0x80)==0)
    {
        opcodetable[opcode].addrmode();
        PC += savepc;
        clockticks6502++;
    }
    else
    {
        value = get6502memory(PC);
        PC++;
    }
}

//-------------------------------------------------------------------------------
void brk6502(void)
{
    PC++;
    put6502memory(0x0100 + SP--, (BYTE)(PC >> 8));
    put6502memory(0x0100 + SP--, (BYTE)(PC & 0xff));
    put6502memory(0x0100 + SP--, P);
    P |= 0x14;
    PC = get6502memory(0xFFFE) + (get6502memory(0xFFFF) << 8);
}

//-------------------------------------------------------------------------------
void bvc6502(void)
{
    if ((P & 0x40)==0)
    {
        opcodetable[opcode].addrmode();
        PC += savepc;
        clockticks6502++;
    }
    else
    {
        value = get6502memory(PC);
        PC++;
    }
}

//-------------------------------------------------------------------------------
void bvs6502(void)
{
    if (P & 0x40)
    {
        opcodetable[opcode].addrmode();
        PC += savepc;
        clockticks6502++;
    }
    else
    {
        value = get6502memory(PC);
        PC++;
    }
}

//-------------------------------------------------------------------------------
void clc6502(void)
{
    P &= 0xfe;
}

//-------------------------------------------------------------------------------
void cld6502(void)
{
    P &= 0xf7;
}

//-------------------------------------------------------------------------------
void cli6502(void)
{
    P &= 0xfb;
}

//-------------------------------------------------------------------------------
void clv6502(void)
{
    P &= 0xbf;
}

//-------------------------------------------------------------------------------
void cmp6502(void)
{
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    if (A + 0x100 - value > 0xff)
    {
        P |= 0x01;
    }
    else
    {
        P &= 0xfe;
    }
    value = A + 0x100 - value;
    if (value)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (value & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void cpx6502(void)
{
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    if (X + 0x100 - value > 0xff)
    {
        P |= 0x01;
    }
    else
    {
        P &= 0xfe;
    }
    value = X + 0x100 - value;
    if (value)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (value & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void cpy6502(void)
{
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    if (Y + 0x100 - value > 0xff)
    {
        P |= 0x01;
    }
    else
    {
        P &= 0xfe;
    }
    value = Y + 0x100 - value;
    if (value)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (value & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void dec6502(void)
{
    uint8 temp;
    opcodetable[opcode].addrmode();
    temp  = get6502memory(savepc);
    temp--;
    put6502memory(savepc, temp);
    value = get6502memory(savepc);
    if (value)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (value & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void dex6502(void)
{
    X--;
    if (X)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (X & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void dey6502(void)
{
    Y--;
    if (Y)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (Y & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void eor6502(void)
{
    opcodetable[opcode].addrmode();
    A ^= get6502memory(savepc);
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void inc6502(void)
{
    uint8 temp;
    opcodetable[opcode].addrmode();
    temp  = get6502memory(savepc);
    temp++;
    put6502memory(savepc,temp);
    value = get6502memory(savepc);
    if (value)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (value & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void inx6502(void)
{
    X++;
    if (X)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (X & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void iny6502(void)
{
    Y++;
    if (Y)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (Y & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void jmp6502(void)
{
    opcodetable[opcode].addrmode();
    PC = savepc;
}

//-------------------------------------------------------------------------------
void jsr6502(void)
{
    PC++;
    put6502memory(0x0100 + SP--, (BYTE)(PC >> 8));
    put6502memory(0x0100 + SP--, (BYTE)(PC & 0xff));
    PC--;
    opcodetable[opcode].addrmode();
    PC = savepc;
}

//-------------------------------------------------------------------------------
void lda6502(void)
{
    opcodetable[opcode].addrmode();
    A = get6502memory(savepc);
    // set the zero flag
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    // set the negative flag
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void ldx6502(void)
{
    opcodetable[opcode].addrmode();
    X = get6502memory(savepc);
    if (X)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (X & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void ldy6502(void)
{
    opcodetable[opcode].addrmode();
    Y = get6502memory(savepc);
    if (Y)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (Y & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void lsr6502(void)
{
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    /* set carry flag if shifting right causes a bit to be lost */
    P = (P & 0xfe) | (value & 0x01);
    value = value >> 1;
    put6502memory(savepc, value);
    /* set zero flag if value is zero */
    if (value != 0)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    /* set negative flag if bit 8 set??? can this happen on an LSR? */
    if ((value & 0x80) == 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void lsra6502(void)
{
    P = (P & 0xfe) | (A & 0x01);
    A = A >> 1;
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void nop6502(void)
{
}

//-------------------------------------------------------------------------------
void ora6502(void)
{
    opcodetable[opcode].addrmode();
    A |= get6502memory(savepc);
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void pha6502(void)
{
    put6502memory(0x100 + SP--, A);
}

//-------------------------------------------------------------------------------
void php6502(void)
{
    put6502memory(0x100 + SP--, P);
}

//-------------------------------------------------------------------------------
void pla6502(void)
{
    A = get6502memory(++SP + 0x100);
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void plp6502(void)
{
    P = get6502memory(++SP + 0x100) | 0x20;
}

//-------------------------------------------------------------------------------
void rol6502(void)
{
    saveflags = (P & 0x01);
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    P = (P & 0xfe) | ((value >> 7) & 0x01);
    value = value << 1;
    value |= saveflags;
    put6502memory(savepc,value);
    if (value)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (value & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void rola6502(void)
{
    saveflags = (P & 0x01);
    P = (P & 0xfe) | ((A >> 7) & 0x01);
    A = A << 1;
    A |= saveflags;
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void ror6502(void)
{
    saveflags = (P & 0x01);
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc);
    P = (P & 0xfe) | (value & 0x01);
    value = value >> 1;
    if (saveflags)
    {
        value |= 0x80;
    }
    put6502memory(savepc,value);
    if (value)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (value & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void rora6502(void)
{
    saveflags = (P & 0x01);
    P = (P & 0xfe) | (A & 0x01);
    A = A >> 1;
    if (saveflags)
    {
        A |= 0x80;
    }
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void rti6502(void)
{
    P   =  get6502memory(++SP + 0x100);
    P  |=  0x20;
    PC  =  get6502memory(++SP + 0x100);
    PC |= (get6502memory(++SP + 0x100) << 8);
}

//-------------------------------------------------------------------------------
void rts6502(void)
{
    PC  =  get6502memory(++SP + 0x100);
    PC |= (get6502memory(++SP + 0x100) << 8);
    PC++;
}

//-------------------------------------------------------------------------------
void sbc6502(void)
{
    opcodetable[opcode].addrmode();
    value = get6502memory(savepc) ^ 0xFF;
    saveflags = (P & 0x01);
    sum = ((char) A) + ((char) value) + (saveflags << 4);
    if ((sum > 0x7f) || (sum < -0x80))
    {
        P |= 0x40;
    }
    else
    {
        P &= 0xbf;
    }
    sum = A + value + saveflags;
    if (sum > 0xff)
    {
        P |= 0x01;
    }
    else
    {
        P &= 0xfe;
    }
    A = sum;
    if (P & 0x08)
    {
        A -= 0x66;
        P &= 0xfe;
        if ((A & 0x0f) > 0x09)
        {
            A += 0x06;
        }
        if ((A & 0xf0) > 0x90)
        {
            A += 0x60;
            P |= 0x01;
        }
    }
    else
    {
        clockticks6502++;
    }
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void sec6502(void)
{
    P |= 0x01;
}

//-------------------------------------------------------------------------------
void sed6502(void)
{
    P |= 0x08;
}

//-------------------------------------------------------------------------------
void sei6502(void)
{
    P |= 0x04;
}

//-------------------------------------------------------------------------------
void sta6502(void)
{
    opcodetable[opcode].addrmode();
    put6502memory(savepc, A);
}

//-------------------------------------------------------------------------------
void stx6502(void)
{
    opcodetable[opcode].addrmode();
    put6502memory(savepc, X);
}

//-------------------------------------------------------------------------------
void sty6502(void)
{
    opcodetable[opcode].addrmode();
    put6502memory(savepc, Y);
}

//-------------------------------------------------------------------------------
void tax6502(void)
{
    X = A;
    if (X)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (X & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void tay6502(void)
{
    Y = A;
    if (Y)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (Y & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void tsx6502(void)
{
    X = SP;
    if (X)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (X & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void txa6502(void)
{
    A = X;
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void txs6502(void)
{
    SP = X;
}

//-------------------------------------------------------------------------------
void tya6502(void)
{
    A = Y;
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void bra6502(void)
{
    opcodetable[opcode].addrmode();
    PC += savepc;
    clockticks6502++;
}

//-------------------------------------------------------------------------------
void dea6502(void)
{
    A--;
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void ina6502(void)
{
    A++;
    if (A)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (A & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void phx6502(void)
{
    put6502memory(0x100 + SP--, X);
}

//-------------------------------------------------------------------------------
void plx6502(void)
{
    X = get6502memory(++SP + 0x100);
    if (X)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (X & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void phy6502(void)
{
    put6502memory(0x100 + SP--, Y);
}

//-------------------------------------------------------------------------------
void ply6502(void)
{
    Y = get6502memory(++SP + 0x100);
    if (Y)
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
    if (Y & 0x80)
    {
        P |= 0x80;
    }
    else
    {
        P &= 0x7f;
    }
}

//-------------------------------------------------------------------------------
void stz6502(void)
{
    opcodetable[opcode].addrmode();
    put6502memory(savepc, 0);
}

//-------------------------------------------------------------------------------
void tsb6502(void)
{
    uint8 temp;
    opcodetable[opcode].addrmode();
    temp  = get6502memory(savepc);
    temp |= A;
    put6502memory(savepc, temp);
    if(get6502memory(savepc))
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
}

//-------------------------------------------------------------------------------
void trb6502(void)
{
    uint8 temp;
    opcodetable[opcode].addrmode();
    temp  =  get6502memory(savepc);
    temp &= (A ^ 0xFF);
    put6502memory(savepc, temp);
    if(get6502memory(savepc))
    {
        P &= 0xfd;
    }
    else
    {
        P |= 0x02;
    }
}

//-------------------------------------------------------------------------------
//CPU复位与中断处理
void reset6502(void)
{
    A = X = Y = P = 0;
    P  |= 0x20;                   //Z_FLAG | R_FLAG
    SP  = 0xff;
    PC  = get6502memory(0xfffc);  // 复位地址，从中读取PC
    PC |= get6502memory(0xfffd) << 8;
    NMI_Flag = CLR0;
    IRQ_Flag = CLR0;
}

//-------------------------------------------------------------------------------
/* Non maskerable interrupt */
void nmi6502(void)
{
    put6502memory(0x0100 + SP--, (BYTE)(PC >> 8));
    put6502memory(0x0100 + SP--, (BYTE)(PC & 0xff));
    put6502memory(0x0100 + SP--, P);
    P  |= 0x04;
    PC  = get6502memory(0xfffa);
    PC |= get6502memory(0xfffb) << 8;
}

//-------------------------------------------------------------------------------
/* Maskerable Interrupt */
void irq6502(void)
{
    put6502memory(0x0100 + SP--, (BYTE)(PC >> 8));
    put6502memory(0x0100 + SP--, (BYTE)(PC & 0xff));
    put6502memory(0x0100 + SP--, P);
    P  |= 0x04;
    PC  = get6502memory(0xfffe);
    PC |= get6502memory(0xffff) << 8;
}

//-------------------------------------------------------------------------------
//执行指令
/* Execute Instruction */
void exec6502(int timerTicks)
{
    /*中断检测*/
    if(NMI_Flag == SET1)
    {
        nmi6502();
        NMI_Flag = CLR0;
    }
    else
    {
        if(IRQ_Flag == SET1 && !(P & I_FLAG))
        {
            irq6502();
            IRQ_Flag = CLR0;
        }
    }
    while (timerTicks > clockticks6502)
    {
        opcode = get6502memory(PC);
        PC++;
        opcodetable[opcode].instruction();
        clockticks6502 += opcodetable[opcode].ticks;
    }
    clockticks6502 -= timerTicks;
}

//-------------------------------------------------------------------------------
/* 初始化6502 存储器*/
void init6502mem( uint8* exp_romptr,
                  uint8* sramptr,
                  uint8* prg_rombank0ptr,
                  uint8  rom_num )
{
    exp_rom        = exp_romptr;                                     /* expansion rom*/
    sram           = sramptr;                                        /* sram*/
    prg_rombank0   = prg_rombank0ptr;                                /* prg-rom lower bank*/
    prg_rombank1   = prg_rombank0ptr + (0x4000 * (rom_num - 1));     /* prg-rom upper bank*/
    clockticks6502 = 0;
}
//-------------------------------------------------------------------------------

#ifndef __NES_H__
#define __NES_H__

#include <xboot.h>

/*
 * Type definition
 */
typedef u32_t	DWORD;
typedef u16_t	WORD;
typedef u8_t	BYTE;


#define RAM_SIZE			0x2000
#define SRAM_SIZE			0x2000
#define PPURAM_SIZE			0x4000
#define SPRRAM_SIZE			256

#define NAME_TABLE0			8
#define NAME_TABLE1			9
#define NAME_TABLE2			10
#define NAME_TABLE3			11

#define NAME_TABLE_V_MASK	2
#define NAME_TABLE_H_MASK	1

#define SPR_Y				0
#define SPR_CHR				1
#define SPR_ATTR			2
#define SPR_X				3
#define SPR_ATTR_COLOR		0x3
#define SPR_ATTR_V_FLIP		0x80
#define SPR_ATTR_H_FLIP		0x40
#define SPR_ATTR_PRI		0x20

#define R0_NMI_VB      0x80
#define R0_NMI_SP      0x40
#define R0_SP_SIZE     0x20
#define R0_BG_ADDR     0x10
#define R0_SP_ADDR     0x08
#define R0_INC_ADDR    0x04
#define R0_NAME_ADDR   0x03

#define R1_BACKCOLOR   0xe0
#define R1_SHOW_SP     0x10
#define R1_SHOW_SCR    0x08
#define R1_CLIP_SP     0x04
#define R1_CLIP_BG     0x02
#define R1_MONOCHROME  0x01

#define R2_IN_VBLANK   0x80
#define R2_HIT_SP      0x40
#define R2_MAX_SP      0x20
#define R2_WRITE_FLAG  0x10

#define SCAN_TOP_OFF_SCREEN				0
#define SCAN_ON_SCREEN					1
#define SCAN_BOTTOM_OFF_SCREEN			2
#define SCAN_UNKNOWN					3
#define SCAN_VBLANK						4

#define SCAN_TOP_OFF_SCREEN_START		0
#define SCAN_ON_SCREEN_START			8
#define SCAN_BOTTOM_OFF_SCREEN_START	232
#define SCAN_UNKNOWN_START				240
#define SCAN_VBLANK_START				243
#define SCAN_VBLANK_END					262

#define STEP_PER_SCANLINE				113
#define STEP_PER_FRAME					29828

/* Develop Scroll Registers */
#define InfoNES_SetupScr() \
	{ \
		/* V-Scroll Register */ \
		/* PPU_Scr_V_Byte_Next = ( BYTE )( ( PPU_Addr & 0x03e0 ) >> 5 ); */ \
		/* PPU_Scr_V_Bit_Next = ( BYTE )( ( PPU_Addr & 0x7000 ) >> 12 ); */ \
		/* H-Scroll Register */ \
		/* PPU_Scr_H_Byte_Next = ( BYTE )( PPU_Addr & 0x001f ); */ \
		/* NameTableBank */ \
		PPU_NameTableBank = NAME_TABLE0 + ( ( PPU_Addr & 0x0C00 ) >> 10 ); \
	}

/* NES display size */
#define NES_DISP_WIDTH					256
#define NES_DISP_HEIGHT					240

#define PAD_SYS_QUIT					0x01
#define PAD_SYS_OK						0x02
#define PAD_SYS_CANCEL					0x04
#define PAD_SYS_UP						0x08
#define PAD_SYS_DOWN					0x10
#define PAD_SYS_LEFT					0x20
#define PAD_SYS_RIGHT					0x40

#define PAD_PUSH(a,b)					(((a) & (b)) != 0)

/*
 * .nes File Header
 */
struct NesHeader_tag
{
	BYTE byID[4];
	BYTE byRomSize;
	BYTE byVRomSize;
	BYTE byInfo1;
	BYTE byInfo2;
	BYTE byReserve[8];
};

/*
 * NES resources
 */
/* RAM */
extern BYTE RAM[];

/* SRAM */
extern BYTE SRAM[];

/* ROM */
extern BYTE * ROM;

/* SRAM BANK ( 8Kb ) */
extern BYTE * SRAMBANK;

/* ROM BANK ( 8Kb * 4 ) */
extern BYTE * ROMBANK0;
extern BYTE * ROMBANK1;
extern BYTE * ROMBANK2;
extern BYTE * ROMBANK3;

/* PPU RAM */
extern BYTE PPURAM[];

/* VROM */
extern BYTE * VROM;

/* PPU BANK ( 1Kb * 16 ) */
extern BYTE * PPUBANK[];

/* Sprite RAM */
extern BYTE SPRRAM[];

/* PPU Register */
extern BYTE PPU_R0;
extern BYTE PPU_R1;
extern BYTE PPU_R2;
extern BYTE PPU_R3;
extern BYTE PPU_R7;

extern BYTE PPU_Scr_V;
extern BYTE PPU_Scr_V_Next;
extern BYTE PPU_Scr_V_Byte;
extern BYTE PPU_Scr_V_Byte_Next;
extern BYTE PPU_Scr_V_Bit;
extern BYTE PPU_Scr_V_Bit_Next;

extern BYTE PPU_Scr_H;
extern BYTE PPU_Scr_H_Next;
extern BYTE PPU_Scr_H_Byte;
extern BYTE PPU_Scr_H_Byte_Next;
extern BYTE PPU_Scr_H_Bit;
extern BYTE PPU_Scr_H_Bit_Next;

extern BYTE PPU_Latch_Flag;
extern WORD PPU_Addr;
extern WORD PPU_Temp;
extern WORD PPU_Increment;

extern BYTE PPU_Latch_Flag;
extern BYTE PPU_UpDown_Clip;

/* Current Scanline */
extern WORD PPU_Scanline;

/* Scanline Table */
extern BYTE PPU_ScanTable[];

/* Name Table Bank */
extern BYTE PPU_NameTableBank;

/* BG Base Address */
extern BYTE *PPU_BG_Base;

/* Sprite Base Address */
extern BYTE *PPU_SP_Base;

/* Sprite Height */
extern WORD PPU_SP_Height;

/* VRAM Write Enable ( 0: Disable, 1: Enable ) */
extern BYTE byVramWriteEnable;

/* Frame IRQ ( 0: Disabled, 1: Enabled )*/
extern BYTE FrameIRQ_Enable;
extern WORD FrameStep;

/* Frame Skip */
extern WORD FrameSkip;
extern WORD FrameCnt;
extern WORD FrameWait;

extern WORD WorkFrame[ NES_DISP_WIDTH * NES_DISP_HEIGHT ];

extern BYTE ChrBuf[];

extern BYTE ChrBufUpdate;

extern WORD PalTable[];

extern BYTE APU_Reg[];
extern BYTE APU_Mute;

extern DWORD PAD1_Latch;
extern DWORD PAD2_Latch;
extern DWORD PAD_System;
extern DWORD PAD1_Bit;
extern DWORD PAD2_Bit;

/*
 * ROM information
 */
/* .nes File Header */
extern struct NesHeader_tag NesHeader;

/* Mapper No. */
extern BYTE MapperNo;

/* Other */
extern BYTE ROM_Mirroring;
extern BYTE ROM_SRAM;
extern BYTE ROM_Trainer;
extern BYTE ROM_FourScr;


void (*MapperInit)();
void (*MapperWrite)(WORD wAddr, BYTE byData);
void (*MapperSram)(WORD wAddr, BYTE byData);
void (*MapperApu)(WORD wAddr, BYTE byData);
BYTE (*MapperReadApu)(WORD wAddr);
void (*MapperVSync)();
void (*MapperHSync)();
void (*MapperPPU)(WORD wAddr);
void (*MapperRenderScreen)(BYTE byMode);

void InfoNES_Init(void);
void InfoNES_Fin(void);
int InfoNES_Load(const char * pszFileName);
int InfoNES_Reset(void);
void InfoNES_SetupPPU(void);
void InfoNES_Mirroring(int nType);
void InfoNES_Main(void);
void InfoNES_Cycle(void);
int InfoNES_HSync(void);
void InfoNES_DrawLine(void);
void InfoNES_GetSprHitY(void);
void InfoNES_SetupChr(void);

#endif /* __NES_H__ */

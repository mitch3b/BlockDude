// let's define some stuff

#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define OAM_ADDRESS		*((unsigned char*)0x2003)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)
#define OAM_DMA			*((unsigned char*)0x4014)


#define RIGHT		0x01
#define LEFT		0x02
#define DOWN		0x04
#define UP			0x08
#define START		0x10
#define SELECT		0x20
#define B_BUTTON	0x40
#define A_BUTTON	0x80

// Globals
// our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char index4;
unsigned char X1;
unsigned char Y1;
unsigned char state;
unsigned char state4;
unsigned char drawGameScreen;
unsigned char gameState;
unsigned char currentMenuOption;
unsigned char joypad1;
unsigned char joypad1old;
unsigned char joypad1test;
unsigned char joypad2;
unsigned char joypad2old;
unsigned char joypad2test;
unsigned char Horiz_scroll;
unsigned char Horiz_scroll_Old;
unsigned int Scroll_Adjusted_X;
// got rid of Vert_scroll, now always 0
unsigned char Nametable;
unsigned char NametableB;


#pragma bss-name(push, "OAM")
unsigned char SPRITES[256]; //64 sprites * 4 bytes
// OAM equals ram addresses 200-2ff


#include "BG/TitleScreen.h"
#include "BG/MenuScreen.h"
#include "BG/GameScreen.h"

// collision maps called C1 and C2
// now their value is 0-11, which will index to this array...

const unsigned char PLATFORM[]={ // which metatiles act like platforms
	0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0};

const unsigned char PALETTE[]={
0x0F, 0x30, 0x07, 0x16, 0x0F, 0x10, 0x00, 0x16,  0x38, 0x39, 0x3A, 0x3B,  0x3C, 0x3D, 0x3E, 0x0F,
0x0F, 0x10, 0x00, 0x16,  0x38, 0x39, 0x3A, 0x3B,  0x3C, 0x3D, 0x3E, 0x0F, 0x0F, 0x30, 0x07, 0x16};

// Prototypes
void All_Off(void);
void All_On(void);
void Reset_Scroll (void);
void Load_Palette(void);
void hide_sprites (void);
void move_logic (void);
void update_sprites (void);
void Draw_Background(void);

void __fastcall__ memcpy (void* dest, const void* src, int count);
void Wait_Vblank(void);
void __fastcall__ UnRLE(const unsigned char *data);
void Get_Input(void);

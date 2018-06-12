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
unsigned char index5;
unsigned char index6;
unsigned char blockCollision;
unsigned char numBlocks;
unsigned char doorX;
unsigned char doorY;
unsigned char X1;
unsigned char Y1;
unsigned char holdingBlock;
unsigned char facingLeft;
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
#include "BG/Level1.h"
#include "BG/Level11.h"
#include "BG/TestLevel.h"

const unsigned char PALETTE[]={
0x00, 0x0F, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30,
0x00, 0x0F, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30};

const unsigned char level_1_blocks_X[] = {0x80, 0xa0};
const unsigned char level_1_blocks_Y[] = {0x80, 0x80};

const unsigned char level_11_blocks_X[] = {0x38, 0x48, 0x50, 0x10, 0x50, 0x90, 0x10, 0x18, 0x88, 0x30, 0x38, 0x80, 0x10, 0xd8, 0x10, 0x18, 0x90, 0x30, 0x98, 0xc0, 0x68, 0xa0, 0x28, 0x58, 0x60, 0x80, 0xa0, 0x80, 0xc8, 0xd0, 0x88, 0x90, 0x98, 0xa8, 0xb0, 0xb8, 0xc0};
const unsigned char level_11_blocks_Y[] = {0x38, 0x38, 0x38, 0x40, 0x40, 0x40, 0x48, 0x48, 0x48, 0x50, 0x50, 0x50, 0x60, 0x60, 0x68, 0x68, 0x68, 0x70, 0x70, 0x70, 0x78, 0x78, 0x80, 0x80, 0x80, 0x90, 0x90, 0x98, 0x98, 0x98, 0xa8, 0xa8, 0xa8, 0xa8, 0xb0, 0xb0, 0xb0};
unsigned char collisionBin[] = {
	0x00, 0x00, 0x00, 0x00, //Row 0
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, // index 11
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, // index 19
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, // index 27
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, //Row 8 - index 35
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, //index 43
	0x02, 0x00, 0x00, 0x40,
	0x02, 0x00, 0x00, 0x40,
	0x02, 0x00, 0x00, 0x40,
	0x02, 0x00, 0x00, 0x40,
	0x02, 0x20, 0x20, 0x40,
	0x02, 0x22, 0x20, 0x40, //Row 16
	0x03, 0xFF, 0xFF, 0xC0,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, //Row 24
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

const unsigned char collisionBin11[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfc, 0x48, 0x80, 0x00, 0x04, 0x40, 0x80, 0x01, 0xf4, 0x43, 0x98, 0x06, 0x54, 0x40, 0xe0, 0x00, 0x54, 0x70, 0x82, 0x00, 0x14, 0x47, 0x81, 0x38, 0xe4, 0x40, 0x01, 0x40, 0x84, 0x40, 0x1d, 0x41, 0x3c, 0x78, 0x39, 0x62, 0x24, 0x40, 0x01, 0xc4, 0x44, 0x40, 0x02, 0x3c, 0x04, 0x43, 0xfe, 0x01, 0xf4, 0x40, 0x00, 0x06, 0x14, 0x78, 0x00, 0x08, 0x14, 0x58, 0x84, 0x00, 0xf4, 0x6e, 0x84, 0x00, 0x04, 0x55, 0x84, 0x00, 0x04, 0x7f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Prototypes
void All_Off(void);
void All_On(void);
void Reset_Scroll (void);
void Load_Palette(void);
void init_test_level(void);
void init_level1(void);
void init_level11(void);
void hide_sprites (void);
void move_logic (void);
void update_sprites (void);
void check_endlevel(void);
void draw_location(void);
void getCollisionIndices(int x, int y);
void add_to_collision_map(int x, int y);

void __fastcall__ memcpy (void* dest, const void* src, int count);
void Wait_Vblank(void);
void __fastcall__ UnRLE(const unsigned char *data);
void Get_Input(void);

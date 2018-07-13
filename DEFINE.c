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
signed char index6;
signed char velocityY;
unsigned char returnValue;
unsigned char numFramesInMovement;
unsigned char buttonBeingHeld;
unsigned char blockCollision;
unsigned char numBlocks;
unsigned char doorX;
unsigned char doorY;
unsigned char X1;
unsigned char Y1;
unsigned char isHoldingBlock;
//TODO these store the password, but could reuse like any other var
unsigned char pwInput[] = {'A','A','A'};
unsigned char holdingBlock;
unsigned char facingLeft;
unsigned char currentLevel;
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

unsigned char Erase_X;
unsigned char Erase_Y;
unsigned char Block_X;
unsigned char Block_Y;

#pragma bss-name(push, "OAM")
unsigned char SPRITES[256]; //64 sprites * 4 bytes
// OAM equals ram addresses 200-2ff


#include "BG/TitleScreen.h"
#include "BG/MenuScreen.h"
#include "BG/HelpScreen.h"
#include "BG/PasswordScreen.h"
#include "BG/GameOverScreen.h"
#include "BG/LevelMenu.h"
#include "BG/Level1.h"
#include "BG/Level2.h"
#include "BG/Level3.h"
#include "BG/Level4.h"
#include "BG/Level5.h"
#include "BG/Level6.h"
#include "BG/Level7.h"
#include "BG/Level8.h"
#include "BG/Level9.h"
#include "BG/Level10.h"
#include "BG/Level1WithBox.h"
#include "BG/Level11.h"
#include "BG/Level11WithBox.h"
#include "BG/TestLevel.h"

const unsigned char PALETTE[]={
0x30, 0x0F, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30,
0x30, 0x0F, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30, 0x0F, 0x00, 0x10, 0x30};

const unsigned char passwords[] = {
	't', 'c', 'P', //	Level 01
	'A', 'R', 'o', // Level 02
	'C', 'K', 's', // Level 03
	'd', 'a', 'N', // Level 04
	'B', 'A', 'H', // Level 05
	'I', 'o', 'n', // Level 06
	'T', 'w', 'e', // Level 07
	'n', 'T', 'y', // Level 08
	'i', 'R', 'C', // Level 09
	'J', 'm', 'K', // Level 10
	'w', 'T', 'F'	 // Level 11
};

const unsigned char menu_option_new[] = {0x28, 3};
const unsigned char menu_option_password[] = {0x48, 8};
const unsigned char menu_option_help[] = {0x90, 4};
const unsigned char menu_option_exit[] = {0xB8, 4};

const unsigned char level_1_blocks_X[] = {0x80, 0xa0};
const unsigned char level_1_blocks_Y[] = {0x80, 0x80};

const unsigned char level_2_blocks_X[] = {0xa8, 0x98, 0xa8, 0xb0, 0x68};
const unsigned char level_2_blocks_Y[] = {0x78, 0x80, 0x80, 0x80, 0x90};

const unsigned char level_3_blocks_X[] = {0xb8, 0xb0, 0xb8, 0xa0, 0x58, 0x60};
const unsigned char level_3_blocks_Y[] = {0x68, 0x70, 0x70, 0x78, 0x88, 0x88};

const unsigned char level_4_blocks_X[] = {0xd0, 0xc8, 0xd0, 0x60, 0x60, 0x80, 0x60, 0x88};
const unsigned char level_4_blocks_Y[] = {0x58, 0x60, 0x60, 0x78, 0x80, 0x80, 0x88, 0x88};

const unsigned char level_5_blocks_X[] = {0x60, 0x68, 0x70, 0x78, 0xc8, 0xc0, 0xc8, 0xb8, 0xc0, 0xc8};
const unsigned char level_5_blocks_Y[] = {0x70, 0x70, 0x70, 0x70, 0x80, 0x88, 0x88, 0x90, 0x90, 0x90};

const unsigned char level_6_blocks_X[] = {0xb8, 0xc0, 0x38, 0x40, 0xa0, 0x38, 0x40, 0x48, 0x98, 0xa0, 0xa8, 0x38, 0x40, 0x48, 0x50, 0x70};
const unsigned char level_6_blocks_Y[] = {0x60, 0x60, 0x68, 0x68, 0x68, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x78, 0x78, 0x78, 0x78, 0x88};

const unsigned char level_7_blocks_X[] = {0xd0, 0xd0, 0xc8, 0xd0, 0x48, 0x58, 0x58, 0x98, 0x58, 0x60, 0x68, 0x98, 0xa0, 0xa8};
const unsigned char level_7_blocks_Y[] = {0x58, 0x60, 0x68, 0x68, 0x70, 0x78, 0x80, 0x80, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88};

const unsigned char level_8_blocks_X[] = {0x20, 0x20, 0x28, 0x50, 0x50, 0xe0, 0xd8, 0xe0, 0x80, 0x40, 0xe0, 0xd8, 0xe0, 0x60, 0xa0, 0xd0, 0xd8, 0xe0};
const unsigned char level_8_blocks_Y[] = {0x48, 0x50, 0x50, 0x68, 0x70, 0x78, 0x80, 0x80, 0x90, 0x98, 0x98, 0xa0, 0xa0, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8};

const unsigned char level_9_blocks_X[] = {0x80, 0x80, 0x88, 0xc0, 0xb8, 0xc0, 0xa0, 0xc0, 0x70};
const unsigned char level_9_blocks_Y[] = {0x50, 0x58, 0x58, 0x58, 0x60, 0x60, 0x70, 0x80, 0x88};

const unsigned char level_10_blocks_X[] = {0x30, 0x70, 0x78, 0x88, 0xa8, 0xb0, 0xb8, 0xc8, 0xa0, 0x60, 0x68, 0x70, 0x78, 0x48, 0x80, 0xd0, 0x30, 0xc8, 0xd0, 0x30, 0x38, 0xc0, 0xc8, 0xd0};
const unsigned char level_10_blocks_Y[] = {0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x40, 0x50, 0x50, 0x50, 0x50, 0x68, 0xa0, 0xa0, 0xa8, 0xa8, 0xa8, 0xb0, 0xb0, 0xb0, 0xb0, 0xb0};

const unsigned char level_11_blocks_X[] = {0x38, 0x48, 0x50, 0x10, 0x50, 0x90, 0x10, 0x18, 0x88, 0x30, 0x38, 0x80, 0x10, 0xd8, 0x10, 0x18, 0x90, 0x30, 0x98, 0xc0, 0x68, 0xa0, 0x28, 0x58, 0x60, 0x80, 0xa0, 0x80, 0xc8, 0xd0, 0x88, 0x90, 0x98, 0xa8, 0xb0, 0xb8, 0xc0};
const unsigned char level_11_blocks_Y[] = {0x38, 0x38, 0x38, 0x40, 0x40, 0x40, 0x48, 0x48, 0x48, 0x50, 0x50, 0x50, 0x60, 0x60, 0x68, 0x68, 0x68, 0x70, 0x70, 0x70, 0x78, 0x78, 0x80, 0x80, 0x80, 0x90, 0x90, 0x98, 0x98, 0x98, 0xa8, 0xa8, 0xa8, 0xa8, 0xb0, 0xb0, 0xb0};

const unsigned char level_test_blocks_X[] = {0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xa0, 0xa8, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98};
const unsigned char level_test_blocks_Y[] = {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8};

//TODO make this more arbitrary to start
unsigned char blocks_X[] = {0x38, 0x48, 0x50, 0x10, 0x50, 0x90, 0x10, 0x18, 0x88, 0x30, 0x38, 0x80, 0x10, 0xd8, 0x10, 0x18, 0x90, 0x30, 0x98, 0xc0, 0x68, 0xa0, 0x28, 0x58, 0x60, 0x80, 0xa0, 0x80, 0xc8, 0xd0, 0x88, 0x90, 0x98, 0xa8, 0xb0, 0xb8, 0xc0};
unsigned char blocks_Y[] = {0x38, 0x48, 0x50, 0x10, 0x50, 0x90, 0x10, 0x18, 0x88, 0x30, 0x38, 0x80, 0x10, 0xd8, 0x10, 0x18, 0x90, 0x30, 0x98, 0xc0, 0x68, 0xa0, 0x28, 0x58, 0x60, 0x80, 0xa0, 0x80, 0xc8, 0xd0, 0x88, 0x90, 0x98, 0xa8, 0xb0, 0xb8, 0xc0};

//TODO I think this is longer than we need...
unsigned char collisionBin[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x20, 0x20, 0x40, 0x02, 0x22, 0xa8, 0x40, 0x03, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char collisionBin1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x20, 0x20, 0x40, 0x02, 0x22, 0xa8, 0x40, 0x03, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x18, 0x06, 0x00, 0x02, 0x00, 0x01, 0x00, 0x06, 0x00, 0x00, 0x80, 0x04, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00, 0x20, 0x02, 0x00, 0x24, 0x20, 0x02, 0x00, 0x36, 0x20, 0x03, 0xe3, 0xff, 0xe0, 0x00, 0x26, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin3[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1f, 0xff, 0x00, 0x02, 0xa0, 0x00, 0x80, 0x02, 0x40, 0x00, 0x80, 0x02, 0x00, 0x01, 0x80, 0x02, 0x00, 0x03, 0x80, 0x02, 0xe0, 0x1b, 0x00, 0x02, 0xa1, 0x3e, 0x00, 0x02, 0xbb, 0x20, 0x00, 0x02, 0xbf, 0x60, 0x00, 0x03, 0xb1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin4[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x10, 0x08, 0x80, 0x00, 0x28, 0x10, 0x40, 0x01, 0xc4, 0x20, 0x20, 0x02, 0x02, 0x40, 0x10, 0x04, 0x01, 0x80, 0x10, 0x04, 0x00, 0x00, 0x30, 0x04, 0x00, 0x00, 0x70, 0x04, 0x00, 0x00, 0x70, 0x0c, 0x20, 0x04, 0x40, 0x08, 0x28, 0x07, 0xc0, 0x0f, 0xa8, 0x9c, 0x00, 0x00, 0xaa, 0xd0, 0x00, 0x00, 0xbf, 0xf0, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin5[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x7f, 0xc0, 0x03, 0xc7, 0x80, 0x20, 0x04, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 0x20, 0x04, 0x10, 0x00, 0x20, 0x04, 0x10, 0x00, 0x20, 0x04, 0x1f, 0x00, 0x20, 0x04, 0x3f, 0x80, 0x20, 0x06, 0xe0, 0xd0, 0x60, 0x02, 0x80, 0x58, 0xe0, 0x02, 0x80, 0x59, 0xe0, 0x03, 0x80, 0x5f, 0xe0, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin6[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x03, 0xc0, 0x02, 0x7f, 0xfc, 0x40, 0x06, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00, 0x40, 0x02, 0x00, 0x01, 0xc0, 0x03, 0x80, 0x49, 0xc0, 0x03, 0xc0, 0x5d, 0x00, 0x03, 0xe0, 0x7d, 0x00, 0x03, 0xe1, 0xc7, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin7[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3e, 0x31, 0xc0, 0x05, 0x41, 0x4a, 0x20, 0x04, 0xc0, 0xc6, 0x10, 0x04, 0x40, 0x42, 0x10, 0x04, 0x00, 0x00, 0x30, 0x04, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x70, 0x08, 0x40, 0x00, 0x70, 0x0c, 0x50, 0x43, 0x40, 0x04, 0x50, 0xd3, 0xc0, 0x06, 0x5c, 0xde, 0x00, 0x02, 0x7e, 0xfe, 0x00, 0x03, 0x43, 0x80, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin8[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x03, 0xc7, 0xf0, 0x11, 0x04, 0x28, 0x08, 0x10, 0x88, 0x30, 0x04, 0x18, 0x70, 0x20, 0xe4, 0x1c, 0x00, 0x01, 0xa4, 0x1e, 0x00, 0x00, 0x24, 0x03, 0x00, 0x0c, 0x34, 0x04, 0x28, 0x12, 0x04, 0x04, 0x34, 0x22, 0x04, 0x08, 0xe2, 0x12, 0x0c, 0x08, 0x14, 0x0c, 0x1c, 0x10, 0x08, 0x00, 0xfc, 0x10, 0x00, 0x80, 0x04, 0x10, 0x81, 0xc0, 0x0c, 0x11, 0xc0, 0x00, 0x1c, 0x10, 0x08, 0x08, 0x3c, 0x1f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin9[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x08, 0x27, 0xc0, 0x00, 0x10, 0x18, 0x40, 0x00, 0x20, 0x80, 0x40, 0x00, 0x40, 0xc0, 0xc0, 0x00, 0x80, 0xe1, 0xc0, 0x01, 0x00, 0x03, 0xc0, 0x02, 0x00, 0x08, 0x40, 0x02, 0x00, 0x1c, 0x40, 0x03, 0x0c, 0x40, 0xc0, 0x01, 0x0e, 0x63, 0xc0, 0x01, 0x0f, 0xe6, 0x00, 0x01, 0xc8, 0x2c, 0x00, 0x00, 0x58, 0x38, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin10[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc0, 0x18, 0x00, 0x80, 0x20, 0x3e, 0x03, 0xc7, 0x70, 0x26, 0x47, 0xcf, 0x68, 0x22, 0x60, 0x1d, 0xc8, 0x23, 0x3f, 0x00, 0x08, 0x20, 0x1f, 0xc0, 0x08, 0x30, 0x08, 0xe0, 0x18, 0x10, 0x45, 0x30, 0x08, 0x10, 0x42, 0x18, 0x08, 0x1e, 0x60, 0x00, 0xf8, 0x07, 0xc0, 0x00, 0x08, 0x04, 0x00, 0x80, 0x08, 0x04, 0x01, 0x87, 0xf8, 0x04, 0x03, 0x00, 0x10, 0x04, 0x00, 0x80, 0x30, 0x06, 0x1f, 0xfc, 0x70, 0x07, 0x30, 0x06, 0xf0, 0x07, 0xe0, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char collisionBin11[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfc, 0x48, 0x80, 0x00, 0x04, 0x41, 0xe0, 0x01, 0xf4, 0x63, 0xb8, 0x26, 0x14, 0x70, 0xe0, 0x40, 0x54, 0x73, 0x82, 0x80, 0x14, 0x47, 0x81, 0x38, 0xe4, 0x60, 0x01, 0x40, 0x94, 0x70, 0x1d, 0x61, 0x3c, 0x7a, 0x39, 0x72, 0xa4, 0x40, 0x05, 0xcc, 0x44, 0x44, 0x1a, 0x3c, 0x04, 0x43, 0xfe, 0x01, 0xf4, 0x40, 0x00, 0x8e, 0x14, 0x78, 0x00, 0x88, 0x74, 0x58, 0x84, 0x00, 0xf4, 0x6e, 0x84, 0x74, 0x04, 0x55, 0x84, 0x03, 0x84, 0x7f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


const unsigned char collisionBinTestLevel[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x20, 0x01, 0xff, 0xfc, 0x10, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf9, 0xff, 0xf0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// Prototypes
void All_Off(void);
void All_On(void);
void Reset_Scroll (void);
void Load_Palette(void);
void init_title_menu(void);
void highlight_menu_option(void);
void init_help_screen(void);
void init_game_over_screen(void);
void help_screen_logic(void);
void init_password_screen(void);
void enter_password_logic(void);
void menu_move_logic(void);
void init_test_level(void);
void init_prelevel_menu(void);
void init_level1(void);
void init_level2(void);
void init_level3(void);
void init_level4(void);
void init_level5(void);
void init_level6(void);
void init_level7(void);
void init_level8(void);
void init_level9(void);
void init_level10(void);
void init_level11(void);
void init_game_complete_screen(void);
void hide_sprites (void);
void testForStart(void);
void move_logic (void);
void update_sprites (void);
void check_endlevel(void);
void draw_location(void);
void getCollisionIndices(int x, int y);
void add_to_collision_map(int x, int y);
void remove_from_background(int x, int y);
void add_block_to_background(int x, int y);
void restart_animation (void);
unsigned char isButtonPressed(unsigned char button);

void init_level(void);

void __fastcall__ memcpy (void* dest, const void* src, int count);
void Wait_Vblank(void);
void __fastcall__ UnRLE(const unsigned char *data);
void Get_Input(void);

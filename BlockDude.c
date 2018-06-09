#include "DEFINE.c"

void main (void) {
	All_Off(); // turn off screen
	Draw_Background();

	gameState = 3; //0 - title, 1 - menu, 2 - scroll, 3 - game

	Load_Palette();
	Reset_Scroll();
	Wait_Vblank();
	All_On(); // turn on screen
	hide_sprites();
	init_level1();
	while (1){ // infinite loop
		while (NMI_flag == 0); // wait till NMI

		//every_frame();	// moved this to the nmi code in reset.s for greater stability
		Get_Input();

		if(gameState == 3) {
			move_logic();
			update_sprites();
		}

		NMI_flag = 0;
	}
}

// inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank

void All_Off (void) {
	PPU_CTRL = 0;
	PPU_MASK = 0;
}

void All_On (void) {
	PPU_CTRL = 0x80; // screen is on, NMI on 1001 0000
	PPU_MASK = 0x1e; // enable all rendering
}

void Reset_Scroll (void) {
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = 0;
	SCROLL = 0;
}

void Load_Palette (void) {
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	PPU_ADDRESS = 0x00;
	for( index = 0; index < sizeof(PALETTE); ++index ){
		PPU_DATA = PALETTE[index];
	}
}

void init_level1(void) {
	X1 = 0x80;
	Y1 = 0x78; 
	
	SPRITES[1] = 0x10;
	SPRITES[2] = 0;
	
	//SPRITES 4-11 for the two blocks
	SPRITES[4] = level1_blocks_Y[0];
	SPRITES[5] = 0x03;
	SPRITES[6] = 0;
	SPRITES[7] = level1_blocks_X[0];
	SPRITES[8] = level1_blocks_Y[1];
	SPRITES[9] = 0x03;
	SPRITES[10] = 0;
	SPRITES[11] = level1_blocks_X[1];
}

void move_logic (void) {
	if(((joypad1 & UP) != 0) && ((joypad1old & UP) == 0)) {
		Y1 = Y1 - 8;
	}
	else if(((joypad1 & DOWN) != 0) && ((joypad1old & DOWN) == 0)) {
		Y1 = Y1 + 8;
	}
	
	if(((joypad1 & RIGHT) != 0) && ((joypad1old & RIGHT) == 0)) {
		X1 = X1 + 8;
	}
	else if(((joypad1 & LEFT) != 0) && ((joypad1old & LEFT) == 0)) {
		X1 = X1 - 8;
	}
}


void update_sprites (void) {
	//0-3 reserved for character
	SPRITES[0] = Y1;
	SPRITES[3] = X1;
	
	
}


void hide_sprites (void) {
	//TODO don't love this but good enough for now
	for (index = 0 ; index < 64 ; index++) {
		SPRITES[4*index] = 0; //set x to offscreen
		SPRITES[4*index + 3] = 0; //set y to offsreen
	}
}

void Draw_Background(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level1);	// uncompresses our data

	PPU_ADDRESS = 0x24; // address of nametable #1 = 0x2400
	PPU_ADDRESS = 0x00;
	UnRLE(MenuScreen);	// uncompresses our data
}

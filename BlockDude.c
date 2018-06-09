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
	holdingBlock = 0;
	facingLeft = 0;
	
	SPRITES[1] = 0x10;
	SPRITES[2] = facingLeft;
	
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
		if(holdingBlock == 0) {
			if(facingLeft == 0) {
				//TODO check if block to the right
				for(index = 1; index  + 1 < sizeof(level1_blocks_X); ++index) {
					//TODO detect if wall above
					if(SPRITES[4*index] == Y1 && (SPRITES[4*index + 3] == (X1 + 8))) {
						holdingBlock = index;
						break;
					}
				}
			}
			else {
				//TODO check if block to the right
				for(index = 1; index  + 1 < sizeof(level1_blocks_X); ++index) {
					//TODO detect if wall above
					if(SPRITES[4*index] == Y1 && (SPRITES[4*index + 3] == (X1 - 8))) {
						holdingBlock = index;
						break;
					}
				}
			}
			
			//TODO don't need this once we have gravity
			if(holdingBlock == 0) {
				Y1 = Y1 + 8;
			}
		}
		else {
			//TODO use gravity
			SPRITES[holdingBlock*4] += 8;
			SPRITES[holdingBlock*4 + 3] += (facingLeft != 0) ? (-8) : 8;
			holdingBlock = 0;
		}
	}
	
	if(((joypad1 & RIGHT) != 0) && ((joypad1old & RIGHT) == 0)) {
		X1 = X1 + 8;
		facingLeft = 0;
	}
	else if(((joypad1 & LEFT) != 0) && ((joypad1old & LEFT) == 0)) {
		X1 = X1 - 8;
		facingLeft = 0x40;
	}
}


void update_sprites (void) {
	//Check against backtround
	if(collision[32*(Y1/8) + X1/8 - 1] == 0) { //TODO this is awful
		SPRITES[0] = Y1;
		SPRITES[2] = facingLeft;
		SPRITES[3] = X1;
	}
	else {
		Y1 = SPRITES[0];
		X1 = SPRITES[3];
	}
	
	//TODO Check against other blocks
	//TODO check diagnol for collision
	//TODO check collision on held block
	
	//Gravity
	/*for(index = 0; index < 3 ; ++index) {
		//TODO reuse collision logic from above
		while(collision[32*((SPRITES[index*4] + 8)/8) + SPRITES[(index*4) + 3]/8 - 1] == 0) {
			SPRITES[index*4] += 8;
		}
	}*/
	//TODO no clue why loop above doesn't work...
	while(collision[32*((SPRITES[0] + 8)/8) + SPRITES[3]/8 - 1] == 0) {
			SPRITES[0] += 8;
	}
	
	while(collision[32*((SPRITES[4] + 8)/8) + SPRITES[7]/8 - 1] == 0) {
			SPRITES[4] += 8;
	}
	
	while(collision[32*((SPRITES[8] + 8)/8) + SPRITES[11]/8 - 1] == 0) {
			SPRITES[8] += 8;
	}
	
	if(holdingBlock != 0) {
		SPRITES[holdingBlock*4] = SPRITES[0] - 8;
		SPRITES[holdingBlock*4 + 3] = SPRITES[3];
	}
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

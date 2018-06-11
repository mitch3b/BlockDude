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
	X1 = 0x70;
	Y1 = 0x70;
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
		getCollisionIndices(SPRITES[3] + (facingLeft ? -8 : 8), SPRITES[0]);

		if((collisionBin[index] & index4) != 0 && (collisionBin[index - 4] & index4) == 0) {
			SPRITES[0] = SPRITES[0] - 8;
			SPRITES[3] = SPRITES[3] + (facingLeft ? -8 : 8);
			Y1 = SPRITES[0];
			X1 = SPRITES[3];
		}
		else {
			collidesWithBlock(SPRITES[3] + (facingLeft ? -8 : 8), SPRITES[0]); //leaves index == 1 if true

			if(blockCollision != 0) {
				SPRITES[0] = SPRITES[0] - 8;
				SPRITES[3] = SPRITES[3] + (facingLeft ? -8 : 8);
				Y1 = SPRITES[0];
				X1 = SPRITES[3];
			}
		}
	}
	else if(((joypad1 & DOWN) != 0) && ((joypad1old & DOWN) == 0)) {
		if(holdingBlock == 0) {
			if(facingLeft == 0) {
				for(index = 1; (index - 1) < sizeof(level1_blocks_X); ++index) {
					//TODO detect if wall above
					if(SPRITES[4*index] == SPRITES[0] && (SPRITES[4*index + 3] == (SPRITES[3] + 8))) {
						holdingBlock = index;
						break;
					}
				}
			}
			else {
				for(index = 1; (index  - 1) < sizeof(level1_blocks_X); ++index) {
					//TODO detect if wall above
					if(SPRITES[4*index] == SPRITES[0] && (SPRITES[4*index + 3] == (SPRITES[3] - 8))) {
						holdingBlock = index;
						break;
					}
				}
			}
		}
		else {
			//TODO make sure there isn't anything in the way
			getCollisionIndices(SPRITES[3] + (facingLeft ? -8 : 8), SPRITES[0] - 8);

			if((collisionBin[index] & index4) == 0) {
				//TODO theres a lot more math than needed here
				SPRITES[holdingBlock*4 + 3] += (facingLeft != 0) ? (-8) : 8;

				//Drop due to gravity (if done anywhere else, you'd need to check if a block has moved before applying gravity)
				getCollisionIndices(SPRITES[holdingBlock*4 + 3], SPRITES[holdingBlock*4] + 8);
				while((collisionBin[index] & index4) == 0) {
						SPRITES[holdingBlock*4] += 8;
						index += 4; //Try directly below
				}
				holdingBlock = 0;
			}
		}
	}
	else if(((joypad1 & RIGHT) != 0) && ((joypad1old & RIGHT) == 0)) {
		X1 = X1 + 8;
		facingLeft = 0;
	}
	else if(((joypad1 & LEFT) != 0) && ((joypad1old & LEFT) == 0)) {
		X1 = X1 - 8;
		facingLeft = 0x40;
	}
}

void getCollisionIndices(int x, int y) {
	index = (y/8);
	index = index*4 + (x/64);

	//Find the bit for that collisionBin Index
	index4 = x % 64;
	index4 = 0x80 >> (index4/8);
}

void collidesWithBlock(int x, int y) {
	for(blockCollision = 1; (blockCollision - 1) < sizeof(level1_blocks_X); ++blockCollision) {
		if(SPRITES[4*blockCollision] == y && SPRITES[4*blockCollision + 3] == x) {
			return;
		}
	}

	blockCollision = 0;
}


void update_sprites (void) {
	//TODO remove for production
	draw_location();
	//Check against background
	getCollisionIndices(X1, Y1);

	if((collisionBin[index] & index4) == 0) {
		collidesWithBlock(X1, Y1); //leaves index == 1 if true

		if(blockCollision == 0) {
			SPRITES[0] = Y1;
			SPRITES[2] = facingLeft;
			SPRITES[3] = X1;
		}
	}
	else {
		//TODO might not need this with it below
		Y1 = SPRITES[0];
		X1 = SPRITES[3];
	}

	//TODO Check against other blocks
	//TODO check diagnol for collision
	//TODO check collision on held block

	getCollisionIndices(SPRITES[3], SPRITES[0] + 8);
	collidesWithBlock(SPRITES[3], SPRITES[0] + 8);
	while((collisionBin[index] & index4) == 0 && blockCollision == 0) {
			SPRITES[0] += 8;
			index += 4; //Try directly below
			collidesWithBlock(SPRITES[3], SPRITES[0] + 8);
	}

	Y1 = SPRITES[0];
	X1 = SPRITES[3];

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
//For debugging
void draw_location(void) {
	//sprites 57-63
	//Get index into collisionBin
	index = holdingBlock;

	//Find the bit for that collisionBin Index
	index4 = facingLeft;
	//x 100's
	SPRITES[228] = 0x10;
	SPRITES[229] = 0xD0 + ((index / 100) % 10);
	SPRITES[230] = 0;
	SPRITES[231] = 0x10;
	//x 10's
	SPRITES[244] = 0x10;
	SPRITES[245] = 0xD0 + ((index / 10) % 10);
	SPRITES[246] = 0;
	SPRITES[247] = 0x18;
	//x 1's
	SPRITES[248] = 0x10;
	SPRITES[249] = 0xD0 + (index % 10);
	SPRITES[250] = 0;
	SPRITES[251] = 0x20;
	//y 100's
	SPRITES[232] = 0x10;
	SPRITES[233] = 0xD0 + ((index4 / 100) % 10);
	SPRITES[234] = 0;
	SPRITES[235] = 0x30;
	//x 10's
	SPRITES[236] = 0x10;
	SPRITES[237] = 0xD0 + ((index4 / 10) % 10);
	SPRITES[238] = 0;
	SPRITES[239] = 0x38;
	//x 1's
	SPRITES[240] = 0x10;
	SPRITES[241] = 0xD0 + (index4 % 10);
	SPRITES[242] = 0;
	SPRITES[243] = 0x40;
}

void Draw_Background(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level1);	// uncompresses our data

	PPU_ADDRESS = 0x24; // address of nametable #1 = 0x2400
	PPU_ADDRESS = 0x00;
	UnRLE(MenuScreen);	// uncompresses our data
}

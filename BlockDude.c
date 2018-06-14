#include "DEFINE.c"

void main (void) {
	All_Off(); // turn off screen
	init_level1();
	gameState = 3; //0 - title, 1 - menu, 2 - scroll, 3 - game
	currentLevel = 1;

	Load_Palette();
	Reset_Scroll();
	Wait_Vblank();
	All_On(); // turn on screen
	while (1){ // infinite loop
		while (NMI_flag == 0); // wait till NMI

		//every_frame();	// moved this to the nmi code in reset.s for greater stability
		Get_Input();

		if(gameState == 3) {
			move_logic();
			update_sprites();
			check_endlevel();
		}
		else if (gameState == 4) {
			currentLevel++;
			
			hide_sprites();
			All_Off();
			
			switch(currentLevel) {
				case 2:
					init_level2();
					break;
				case 3:
					init_level3();
					break;
				case 4:
					init_level11();
					break;
			}
			
			Wait_Vblank();
			All_On();
			Reset_Scroll();
			
			gameState = 3;
		}
		//TODO remove ... just for debugging
		draw_location();

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

void init_level(const unsigned char level_blocks_x[], const unsigned char level_blocks_y[]) {
	SPRITES[4] = 0;
	SPRITES[5] = 0x03;
	SPRITES[6] = 0;
	SPRITES[7] = 0;
	
	holdingBlock = 0;
	isHoldingBlock = 0;
	facingLeft = 0;
	
	SPRITES[1] = 0x10;
	SPRITES[2] = facingLeft;
	
	numBlocks = sizeof(level_blocks_x);
	
	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_blocks_x[index5];
		blocks_Y[index5] = level_blocks_y[index5];

		//TODO make this part of the collision map to begin with
		add_to_collision_map(level_blocks_x[index5], level_blocks_y[index5]);
	}
}

void init_level1(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level1WithBox);	// uncompresses our data

	X1 = 0xB0;//TODO this isnt right
	Y1 = 0x70;
	doorX = 0x38;
	doorY = 0x80;
	
	init_level(level_1_blocks_X, level_1_blocks_Y);
}

void init_level2(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level2);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin2) ; index++) {
		collisionBin[index] = collisionBin2[index];
	}

	X1 = 0xB8;
	Y1 = 0x80;
	doorX = 0x30;
	doorY = 0x68;
	
	init_level(level_2_blocks_X[index5], level_2_blocks_Y[index5]);
}

void init_level3(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level3);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin3) ; index++) {
		collisionBin[index] = collisionBin3[index];
	}

	X1 = 0x78;
	Y1 = 0x78;
	doorX = 0x38;
	doorY = 0x90;
	
	init_level(level_3_blocks_X[index5], level_3_blocks_Y[index5]);
}

void init_level11(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level11WithBox);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin11) ; index++) {
		collisionBin[index] = collisionBin11[index];
	}

	X1 = 0x70;
	Y1 = 0x48;
	doorX = 0xC8;
	doorY = 0x40;
	
	init_level(level_11_blocks_X[index5], level_11_blocks_Y[index5]);
}


void add_to_collision_map(int x, int y) {
	getCollisionIndices(x, y);
	collisionBin[index] = index4 | collisionBin[index];
}

void remove_from_collision_map(int x, int y) {
	getCollisionIndices(x, y);
	collisionBin[index] = index4 ^ collisionBin[index];
}

void remove_from_background(int x, int y) {
	index = y/8;
	//Nametable 0
	PPU_ADDRESS = 0x20 + (index/8); 
	PPU_ADDRESS = 32*(index % 4) + (x/8); 
	PPU_DATA = 0x01;
	Reset_Scroll();
}

//TODO might want to consolidate with above
void add_block_to_background(int x, int y) {
	index = y/8;
	//Nametable 0
	PPU_ADDRESS = 0x20 + (index/8); 
	PPU_ADDRESS = 32*(index % 8) + (x/8); 
	PPU_DATA = 0x03;
	Reset_Scroll();
}

void move_logic (void) {
	if(((joypad1 & UP) != 0) && ((joypad1old & UP) == 0)) {
		//See if we can move up in the direction we're facing
		X1 = SPRITES[3] + (facingLeft ? -8 : 8);
		Y1 = SPRITES[0] - 8;

		//TODO can save some of the iterating blocks if we don't check when we know background check passed
		getCollisionIndices(X1, SPRITES[0]);

		//If block next to you can be stood on
		if((collisionBin[index] & index4) != 0 && //something on side to stand on
		   (collisionBin[index - 4] & index4) == 0 && //nothing above it to block you
		   (isHoldingBlock == 0 || (collisionBin[index - 8] & index4) == 0)){

			//Check diagnols
			getCollisionIndices(SPRITES[3], SPRITES[0]);

			if((collisionBin[index - 4] & index4) == 0 && //nothing above you to block on diagnol
			   (isHoldingBlock == 0 || (collisionBin[index - 8] & index4) == 0)){
				//TODO Make sure the diagnol isn't occupied
				SPRITES[0] = Y1;
				SPRITES[3] = X1;
			}
			else {
				//TODO should be a better way, but the update sprites gets in the way
				X1 = SPRITES[3];
				Y1 = SPRITES[0];
			}
		}
		else {
			//TODO should be a better way, but the update sprites gets in the way
			X1 = SPRITES[3];
			Y1 = SPRITES[0];
		}
	}
	else if(((joypad1 & DOWN) != 0) && ((joypad1old & DOWN) == 0)) {			
		if(isHoldingBlock == 0) {
							
			//Check above player is empty
			getCollisionIndices(SPRITES[3], SPRITES[0] - 8);

			if((collisionBin[index] & index4) == 0) {
				index6 = (facingLeft == 0) ? 8 : -8;
				//Check above block is empty
				getCollisionIndices(SPRITES[3] + index6, SPRITES[0] - 8);

				if((collisionBin[index] & index4) == 0) {
					//Check if block is there to pick up
					for(index5 = 0; index5 < numBlocks; ++index5) {
						if(blocks_Y[index5] == SPRITES[0] && (blocks_X[index5] == (SPRITES[3] + index6))) {
							remove_from_collision_map(blocks_X[index5], blocks_Y[index5]);
							remove_from_background(blocks_X[index5], blocks_Y[index5]);
							holdingBlock = index5; 
							isHoldingBlock = 1;
							break;
						}
					}
				}
			}
		}
		else {
			//holding a block so check if we can put one down
			//TODO make sure there isn't anything in the way
			getCollisionIndices(SPRITES[3] + (facingLeft ? -8 : 8), SPRITES[0] - 8);

			if((collisionBin[index] & index4) == 0) {
				blocks_X[holdingBlock] = SPRITES[7];
				blocks_Y[holdingBlock] = SPRITES[4];
				
				blocks_X[holdingBlock] += (facingLeft != 0) ? (-8) : 8;

				//Drop due to gravity (if done anywhere else, you'd need to check if a block has moved before applying gravity)
				getCollisionIndices(blocks_X[holdingBlock], blocks_Y[holdingBlock] + 8);
				while((collisionBin[index] & index4) == 0 && blockCollision == 0) {
						blocks_Y[holdingBlock] += 8;
						index += 4; //Try directly below
				}

				add_to_collision_map(blocks_X[holdingBlock], blocks_Y[holdingBlock]);
				add_block_to_background(blocks_X[holdingBlock], blocks_Y[holdingBlock]);
				SPRITES[4] = 0;
				SPRITES[7] = 0;
				isHoldingBlock = 0;
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

//Changes index to where in collision bin, index4 to which bit
void getCollisionIndices(int x, int y) {
	index = (y/8);
	index = index*4 + (x/64);

	//Find the bit for that collisionBin Index
	index4 = x % 64;
	index4 = 0x80 >> (index4/8);
}


void update_sprites (void) {
	//Check against background
	getCollisionIndices(X1, Y1);

	if((collisionBin[index] & index4) == 0 &&
		(isHoldingBlock == 0 || (collisionBin[index - 4] & index4) == 0)) {
		SPRITES[0] = Y1;
		SPRITES[2] = facingLeft;
		SPRITES[3] = X1;
	}
	else {
		//TODO might not need this with it below
		Y1 = SPRITES[0];
		X1 = SPRITES[3];
	}

	//Fall if need be
	getCollisionIndices(SPRITES[3], SPRITES[0] + 8);
	while((collisionBin[index] & index4) == 0 && blockCollision == 0) {
			SPRITES[0] += 8;
			index += 4; //Try directly below
	}

	Y1 = SPRITES[0];
	X1 = SPRITES[3];

	if(isHoldingBlock != 0) {
		SPRITES[4] = SPRITES[0] - 8;
		SPRITES[7] = SPRITES[3];
	}
}


void hide_sprites (void) {
	//TODO don't love this but good enough for now
	for (index = 0 ; index < 64 ; index++) {
		SPRITES[4*index] = 0; //set x to offscreen
		SPRITES[4*index + 3] = 0; //set y to offsreen
	}
}

void check_endlevel(void) {
	if(SPRITES[0] == doorY && SPRITES[3] == doorX) {
		//Level complete
		gameState = 4;
	}
}

//For debugging
void draw_location(void) {
	//sprites 57-63
	//Get index into collisionBin
	index = blocks_X[1];

	//Find the bit for that collisionBin Index
	index4 = blocks_Y[1];
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

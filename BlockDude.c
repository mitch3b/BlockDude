#include "DEFINE.c"

void main (void) {
	All_Off(); // turn off screen
	gameState = 1; //0 - title, 1 - load menu, 2 in menu, 3 - load level menu, 4 - wait for start, 5 load level, 6 in level,
	               //7 in restart animation, 8 load password screen, 9 password screen, 10 load help screen, 11 - help screen
								 //12 - game over screen, 13 - stuck in game over
	currentLevel = 1;

	Load_Palette();
	Reset_Scroll();
	Wait_Vblank();
	All_On(); // turn on screen
	while (1){ // infinite loop
		while (NMI_flag == 0); // wait till NMI

		//every_frame();	// moved this to the nmi code in reset.s for greater stability
		Get_Input();

		if(gameState == 14) {
			//Do nothing, game over screen or game complete screen is up.
		}
		else if(gameState == 13) {
			//Do nothing, game over screen or game complete screen is up.
		}
		else if(gameState == 12) {
			All_Off();

			init_game_over_screen();

			Wait_Vblank();
			All_On();
			Reset_Scroll();
		}
		else if(gameState == 11) {
			help_screen_logic();
		}
		else if(gameState == 10) {
			All_Off();

			init_help_screen();

			Wait_Vblank();
			All_On();
			Reset_Scroll();
		}
		else if(gameState == 9) {
			enter_password_logic();
		}
		else if(gameState == 8) {
			All_Off();

			init_password_screen();

			Wait_Vblank();
			All_On();
			Reset_Scroll();
		}
		else if(gameState ==7) {
			restart_animation();
		}
		else if(gameState == 6) {
			move_logic();
			update_sprites();
			check_endlevel();
		}
		else if (gameState == 5) { //Load level
			All_Off();

			gameState = 6; //By default go to in level state

			switch(currentLevel) {
				case 1:
					init_level1();
					break;
				case 2:
					init_level2();
					break;
				case 3:
					init_level3();
					break;
				case 4:
					init_level4();
					break;
				case 5:
					init_level5();
					break;
				case 6:
					init_level6();
					break;
				case 7:
					init_level7();
					break;
				case 8:
					init_level8();
					break;
				case 9:
					init_level9();
					break;
				case 10:
					init_level10();
					break;
				case 11:
					init_level11();
					break;
				case 12:
					gameState = 14;
					init_game_complete_screen();
					break;
				default:
					init_level1();
			}

			Wait_Vblank();
			All_On();
			Reset_Scroll();
		}
		else if (gameState == 3) { //Pre level menu
			hide_sprites();
			All_Off();

			init_prelevel_menu();

			Wait_Vblank();
			All_On();
			Reset_Scroll();
			gameState = 4;
		}
		else if (gameState == 4) {
			testForStart();
		}
		else if (gameState == 1) {
			All_Off();
			init_title_menu();
			Wait_Vblank();
			All_On();
			Reset_Scroll();

			gameState = 2;
			currentMenuOption = 0; // 0 New, 1 password, 2 help, 3 exit
			highlight_menu_option();
		}
		else if (gameState == 2) {
			menu_move_logic();
			highlight_menu_option();
		}
		//TODO remove ... just for debugging
		//draw_location();

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

void init_level(void) {
	hide_sprites();

	SPRITES[4] = 0;
	SPRITES[5] = 0x03;
	SPRITES[6] = 0;
	SPRITES[7] = 0;

	holdingBlock = 0;
	isHoldingBlock = 0;
	facingLeft = 0;
	Block_X = 0;
	Block_Y = 0;
	Erase_X = 0;
	Erase_Y = 0;

	SPRITES[1] = 0x10;
	SPRITES[2] = facingLeft;

	numFramesToMove = 10; //When holding a button
}

void init_test_level(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(TestLevel);	// uncompresses our data


	for(index = 0 ; index < sizeof(collisionBinTestLevel) ; index++) {
		collisionBin[index] = collisionBinTestLevel[index];
	}

	X1 = 0xB0;
	Y1 = 0x70;
	doorX = 0x00;
	doorY = 0x00;

	init_level();
	numBlocks = sizeof(level_test_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_test_blocks_X[index5];
		blocks_Y[index5] = level_test_blocks_Y[index5];
	}
}

void init_title_menu(void) {
	hide_sprites();
	//Else you'll get a rogue block drawn :)
	Erase_X = 0;
	Erase_Y = 0;
	Block_X = 0;
	Block_Y = 0;

	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(MenuScreen);	// uncompresses our data
}

void highlight_menu_option(void) {
	hide_sprites();//Can be more efficient
	//First 8 sprites are for highlighting.

	switch(currentMenuOption) {
		case 0:
			index = menu_option_new[0];//Starting X
			index6 = menu_option_new[1];//Num sprites
			break;
		case 1:
			index = menu_option_password[0];//Starting X
			index6 = menu_option_password[1];//Num sprites
			break;
		case 2:
			index = menu_option_help[0];//Starting X
			index6 = menu_option_help[1];//Num sprites
			break;
		case 3:
			index = menu_option_exit[0];//Starting X
			index6 = menu_option_exit[1];//Num sprites
			break;
	}

	index4 = 0;
	for(;index6 > 0 ; index6--) {
		SPRITES[index4] = 0xB0;
		index4++;
		SPRITES[index4] = 0x06;
		index4++;
		SPRITES[index4] = 0x20;//TODO set to behind background
		index4++;
		SPRITES[index4] = index;
		index4++;
		index += 8;
	}
}

void init_prelevel_menu(void) {
	hide_sprites();
	//Else you'll get a rogue block drawn :)
	Erase_X = 0;
	Erase_Y = 0;
	Block_X = 0;
	Block_Y = 0;

	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(LevelMenu);	// uncompresses our data

	//Level
	SPRITES[0]  = 0x68;
	SPRITES[1]  = (currentLevel > 9) ? (0x30 + (currentLevel / 10)) : 0x01; //Only show leading digit if it exists
	SPRITES[2]  = 0x00;
	SPRITES[3]  = 0x80;

	SPRITES[4]  = 0x68;
	SPRITES[5]  = 0x30 + (currentLevel % 10);
	SPRITES[6]  = 0x00;
	SPRITES[7]  = 0x88;

	index4 = 8;
	index5 = 0x98;
	for(index = 3*(currentLevel - 1) ; index4 < 20 ; ++index) {
		SPRITES[index4]  = 0x78;
		index4++;
		SPRITES[index4]  = passwords[index];
		index4++;
		SPRITES[index4]  = 0x00;
		index4++;
		SPRITES[index4]  = index5;
		index4++;
		index5 += 8;
	}
}

void init_game_over_screen(void) {
		PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
		PPU_ADDRESS = 0x00;
		UnRLE(GameOverScreen);	// uncompresses our data

		hide_sprites();
		gameState = 13;
}

void init_help_screen(void) {
		PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
		PPU_ADDRESS = 0x00;
		UnRLE(HelpScreen);	// uncompresses our data

		hide_sprites();
		gameState = 11;
}

void help_screen_logic(void) {
	//on any button go back to menu
	if(joypad1 != 0 && joypad1old == 0) {
		gameState = 1;
	}
}

void init_password_screen(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(PasswordScreen);	// uncompresses our data

	hide_sprites();

	numFramesToMove = 5;

	pwInput[0] = 'A';
	pwInput[1] = 'A';
	pwInput[2] = 'A';
	index6 = 0;  //Used to count how many letters inputted

	SPRITES[0] = 0x88;
	SPRITES[1] = pwInput[0];
	SPRITES[2] = 0;
	SPRITES[3] = 0x78;
	SPRITES[4] = 0x88;
	SPRITES[5] = pwInput[1];
	SPRITES[6] = 0;
	SPRITES[7] = 0x80;
	SPRITES[8] = 0x88;
	SPRITES[9] = pwInput[2];
	SPRITES[10] = 0;
	SPRITES[11] = 0x88;

	gameState = 9;
}

void enter_password_logic(void){
	if(isButtonPressed(A_BUTTON) || isButtonPressed(START) || isButtonPressed(RIGHT)){
		index6++;
	}
	else if(isButtonPressed(B_BUTTON) || isButtonPressed(LEFT)) {
		if(index6 > 0) {
			index6--;
		}
	}
	else if(isButtonPressed(UP)) {
		buttonBeingHeld = UP;
		numFramesInMovement = 0;

		pwInput[index6]++;
		if(pwInput[index6] > 'Z' && pwInput[index6] < 'a') {
			pwInput[index6] = 'a';
		}
		else if(pwInput[index6] > 'z') {
			pwInput[index6] = 'A';
		}
	}
	else if(isButtonPressed(DOWN)) {
		buttonBeingHeld = DOWN;
		numFramesInMovement = 0;

		pwInput[index6]--;
		if(pwInput[index6] > 'Z' && pwInput[index6] < 'a') {
			pwInput[index6] = 'Z';
		}
		else if(pwInput[index6] < 'A') {
			pwInput[index6] = 'z';
		}
	}

	numFramesInMovement++;

	SPRITES[1] = pwInput[0];
	SPRITES[5] = (index6 > 0) ? pwInput[1] : 0x01;
	SPRITES[9] = (index6 > 1) ? pwInput[2] : 0x01;;

	//If all chars inputted, then check if pw matches anything
	if(index6 == 3) {
		facingLeft = 0;
		currentLevel = 1;
		for(index6 = 0; index6 < sizeof(passwords); ) {
			facingLeft = 0;
			facingLeft += (passwords[index6] == pwInput[0]) ? 1 : 0;
			index6++;
			facingLeft += (passwords[index6] == pwInput[1]) ? 1 : 0;
			index6++;
			facingLeft += (passwords[index6] == pwInput[2]) ? 1 : 0;
			index6++;

			if(facingLeft == 3) {
				currentLevel = (index6/3);
			}
		}

		gameState = 3;
	}
}

void init_level1(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level1WithBox);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin1) ; index++) {
		collisionBin[index] = collisionBin1[index];
	}

	X1 = 0xB0;//TODO this isnt right
	Y1 = 0x70;
	doorX = 0x38;
	doorY = 0x80;

	init_level();
	numBlocks = sizeof(level_1_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_1_blocks_X[index5];
		blocks_Y[index5] = level_1_blocks_Y[index5];
	}
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

	init_level();
	numBlocks = sizeof(level_2_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_2_blocks_X[index5];
		blocks_Y[index5] = level_2_blocks_Y[index5];
	}
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

	init_level();
	numBlocks = sizeof(level_3_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_3_blocks_X[index5];
		blocks_Y[index5] = level_3_blocks_Y[index5];
	}
}

void init_level4(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level4);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin4) ; index++) {
		collisionBin[index] = collisionBin4[index];
	}

	X1 = 0xA8;
	Y1 = 0x68;
	doorX = 0x28;
	doorY = 0x78;

	init_level();
	numBlocks = sizeof(level_4_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_4_blocks_X[index5];
		blocks_Y[index5] = level_4_blocks_Y[index5];
	}
}


void init_level5(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level5);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin5) ; index++) {
		collisionBin[index] = collisionBin5[index];
	}

	X1 = 0x88;
	Y1 = 0x78;
	doorX = 0x30;
	doorY = 0x78;

	init_level();
	numBlocks = sizeof(level_5_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_5_blocks_X[index5];
		blocks_Y[index5] = level_5_blocks_Y[index5];
	}
}


void init_level6(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level6);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin6) ; index++) {
		collisionBin[index] = collisionBin6[index];
	}

	X1 = 0x90;
	Y1 = 0x70;
	doorX = 0x30;
	doorY = 0x50;

	init_level();
	numBlocks = sizeof(level_6_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_6_blocks_X[index5];
		blocks_Y[index5] = level_6_blocks_Y[index5];
	}
}

void init_level7(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level7);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin7) ; index++) {
		collisionBin[index] = collisionBin7[index];
	}

	X1 = 0xA8;
	Y1 = 0x80;
	doorX = 0x28;
	doorY = 0x70;

	init_level();
	numBlocks = sizeof(level_7_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_7_blocks_X[index5];
		blocks_Y[index5] = level_7_blocks_Y[index5];
	}
}

void init_level8(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level8);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin8) ; index++) {
		collisionBin[index] = collisionBin8[index];
	}

	X1 = 0xB8;
	Y1 = 0xA8;
	doorX = 0xD8;
	doorY = 0x58;

	init_level();
	numBlocks = sizeof(level_8_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_8_blocks_X[index5];
		blocks_Y[index5] = level_8_blocks_Y[index5];
	}
}

void init_level9(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level9);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin9) ; index++) {
		collisionBin[index] = collisionBin9[index];
	}

	X1 = 0xA0;
	Y1 = 0x68;
	doorX = 0x38;
	doorY = 0x78;

	init_level();
	numBlocks = sizeof(level_9_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_9_blocks_X[index5];
		blocks_Y[index5] = level_9_blocks_Y[index5];
	}
}

void init_level10(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(Level10);	// uncompresses our data

	for(index = 0 ; index < sizeof(collisionBin10) ; index++) {
		collisionBin[index] = collisionBin10[index];
	}

	X1 = 0x80;
	Y1 = 0x80;
	doorX = 0x18;
	doorY = 0x58;

	init_level();
	numBlocks = sizeof(level_10_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_10_blocks_X[index5];
		blocks_Y[index5] = level_10_blocks_Y[index5];
	}
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

	init_level();
	numBlocks = sizeof(level_11_blocks_X);

	index6 = 4;
	for(index5 = 0; index5 < numBlocks; ++index5) {
		blocks_X[index5] = level_11_blocks_X[index5];
		blocks_Y[index5] = level_11_blocks_Y[index5];
	}
}

void init_game_complete_screen(void) {
	PPU_ADDRESS = 0x20; // address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	UnRLE(GameCompleteScreen);	// uncompresses our data

	X1 = 0;
	Y1 = 0;

	init_level();
	hide_sprites();
}


void add_to_collision_map(int x, int y) {
	getCollisionIndices(x, y);
	collisionBin[index] = index4 | collisionBin[index];
}

void remove_from_collision_map(int x, int y) {
	getCollisionIndices(x, y);
	collisionBin[index] = index4 ^ collisionBin[index];
}

//TODO 120 is a problem
void remove_from_background(int x, int y) {
	index = y/8;
	Erase_Y = 0x20 + (index/8);
	Erase_X = 32*(index % 8) + (x/8);
}

//TODO might want to consolidate with above
void add_block_to_background(int x, int y) {
	index = y/8;
	Block_Y = 0x20 + (index/8);
	Block_X = 32*(index % 8) + (x/8);

	//In case we're adding the block to where we just erased it (erase happens second)
	Erase_Y = 0;
	Erase_X = 0;
}

void testForStart(void) {
	if(((joypad1 & START) != 0) && ((joypad1old & START) == 0)) {
		gameState = 5;
	}
}

void menu_move_logic(void) {
	if(((joypad1 & RIGHT) != 0) && ((joypad1old & RIGHT) == 0)) {
		currentMenuOption = (currentMenuOption + 1) % 4;
	}
	else if(((joypad1 & LEFT) != 0) && ((joypad1old & LEFT) == 0)) {
		if(currentMenuOption == 0) {
			currentMenuOption = 3;
		}
		else {
			currentMenuOption--;
		}
	}

	if(((joypad1 & START) != 0) && ((joypad1old & START) == 0)) {
		if(currentMenuOption == 0) {
			gameState = 3;
		}
		else if (currentMenuOption == 1) {
			gameState = 8;
		}
		else if (currentMenuOption == 2) {
			gameState = 10;
		}
		else if(currentMenuOption == 3) {
			gameState = 12;
		}
	}
}

void restart_animation (void) {
	if(SPRITES[0] < 10) {
		//Animation complete
		gameState = 5;
		SPRITES[2] = 0; //Reset character to rightside up... not sure if we need this but safe
	}
	else {
		SPRITES[0] -= velocityY/5;
		velocityY -= 1;

		if(velocityY < 0) {
			SPRITES[2] = 0x80;
		}

		if(velocityY < -50) {
			velocityY = -50;
		}
	}
}

void move_logic (void) {
	if(isButtonPressed(SELECT)) {
		//Reset on select
		velocityY = 10;
		gameState = 7;
	}

	if(isButtonPressed(UP)) {
		buttonBeingHeld = UP;
		numFramesInMovement = 0;

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
	else if(isButtonPressed(B_BUTTON)) {
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
	else if(isButtonPressed(RIGHT)) {
		X1 = X1 + 8;
		facingLeft = 0;
		buttonBeingHeld = RIGHT;
		numFramesInMovement = 0;
	}
	else if(isButtonPressed(LEFT)) {
		X1 = X1 - 8;
		facingLeft = 0x40;
		buttonBeingHeld = LEFT;
		numFramesInMovement = 0;
	}
	else if (joypad1 == 0) {
		buttonBeingHeld = 3; //NOT A VALID BUTTON
	}

	numFramesInMovement++;
}

unsigned char isButtonPressed(unsigned char button) {
	return (((joypad1 & button) != 0) &&
	          ((buttonBeingHeld == button && numFramesInMovement > numFramesToMove) ||
			   ((joypad1old & button) == 0)));
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

void check_endlevel(void) {
	if(SPRITES[0] == doorY && SPRITES[3] == doorX) {
		//Level complete
		currentLevel++;
		if(currentLevel < 12) {
			gameState = 3;
		}
		else {
			gameState = 5;
		}
	}
}

//For debugging
void draw_location(void) {
	//sprites 228-243
	//Get index into collisionBin
	index = numFramesInMovement;

	//Find the bit for that collisionBin Index
	index4 = buttonBeingHeld;
	//x 100's
	SPRITES[228] = 0x10;
	SPRITES[229] = 0x30 + ((index / 100) % 10);
	SPRITES[230] = 0;
	SPRITES[231] = 0x10;
	//x 10's
	SPRITES[244] = 0x10;
	SPRITES[245] = 0x30 + ((index / 10) % 10);
	SPRITES[246] = 0;
	SPRITES[247] = 0x18;
	//x 1's
	SPRITES[248] = 0x10;
	SPRITES[249] = 0x30 + (index % 10);
	SPRITES[250] = 0;
	SPRITES[251] = 0x20;
	//y 100's
	SPRITES[232] = 0x10;
	SPRITES[233] = 0x30 + ((index4 / 100) % 10);
	SPRITES[234] = 0;
	SPRITES[235] = 0x30;
	//x 10's
	SPRITES[236] = 0x10;
	SPRITES[237] = 0x30 + ((index4 / 10) % 10);
	SPRITES[238] = 0;
	SPRITES[239] = 0x38;
	//x 1's
	SPRITES[240] = 0x10;
	SPRITES[241] = 0x30 + (index4 % 10);
	SPRITES[242] = 0;
	SPRITES[243] = 0x40;
}

level = "10"
with open("BG/Level" + level + "Collision.h") as f:
	lineCount = 0
	outputX = "const unsigned char level_" + level + "_blocks_X[] = {"
	outputY = "const unsigned char level_" + level + "_blocks_Y[] = {"
	for line in f:
		tokens = line.split(',')
		if(len(tokens) == 17):
			for i in range(0, 16):
				num = tokens[i][2:]
				if(num == "03"):
					outputX += "0x" +  hex(i*8 + 128*(lineCount%2))[2:].zfill(2) + ", "
					outputY += "0x" +  hex(int(lineCount/2)*8)[2:].zfill(2) + ", "
			lineCount += 1
		
	outputX += "};"
	outputY += "};"
		
	print(outputX)
	print(outputY)
			# const unsigned char level__blocks_X[] = {0x38, 0x48, 0x50, 0x48, 0x48};
			# 0x90, 0x40

with open("/Users/mitchken/Documents/NesDev/BlockDude/BG/Level3Collision.h") as f:
	for line in f:
		tokens = line.split(',')
		midLine = 0;
		if(len(tokens) == 17):
			tempNum1 = ""
			for i in range(0, 8):
				num = tokens[i][2:]
				if(num == "01" or num == "04"):
					tempNum1 += "0"
				else:
				    tempNum1 += "1"
			tempNum1 = hex(int(tempNum1, 2))[2:].zfill(2)
			tempNum2 = ""
			for i in range(8, 16):
				num = tokens[i][2:]
				if(num == "01" or num == "04"):
					tempNum2 += "0"
				else:
				    tempNum2 += "1"
			tempNum2 = hex(int(tempNum2, 2))[2:].zfill(2)

			if(midLine == 0):
				midLine = 1
				print("0x" + tempNum1 + ", 0x" + tempNum2 + ", " )
			else:
				midLine = 0
				print("0x" + tempNum1 + ", 0x" + tempNum2 + "," )

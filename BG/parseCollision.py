level = "10"
with open("BG/Level" + level + "Collision.h") as f:
	output = "const unsigned char collisionBin" + level + "[] = {";
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

			output += "0x" + tempNum1 + ", 0x" + tempNum2 + ", "

	output += "};"
	print(output)
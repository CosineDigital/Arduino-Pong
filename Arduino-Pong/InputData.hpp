#pragma once

struct InputData final {
	bool resetPressed;
	bool p1UpPressed;
	bool p1DownPressed;
	bool p2UpPressed;
	bool p2DownPressed;
};

// data structure:
//	0		0		0		1		1		1		1		1
//							reset	leftup	leftd	rigthup	rightdown
static InputData byteToInputData(char byte) {
	return {
		(bool) (byte & 1 << 5),
		(bool) (byte & 1 << 4),
		(bool) (byte & 1 << 3),
		(bool) (byte & 1 << 2),
		(bool) (byte & 1 << 0)
	};
}
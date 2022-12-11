#pragma once

// data structure:
//	0		0		0		1		1		1		1		1
//							reset	leftup	leftd	rigthup	rightdown
struct InputData final {
	bool resetPressed;
	bool p1Up;
	bool p1Down;
	bool p2Up;
	bool p2Down;
};

static InputData byteToInput(char byte) {
	return {
		(bool) (byte & 0b00010000),
		(bool) (byte & 0b00001000),
		(bool) (byte & 0b00000100),
		(bool) (byte & 0b00000010),
		(bool) (byte & 0b00000001)
	};
}
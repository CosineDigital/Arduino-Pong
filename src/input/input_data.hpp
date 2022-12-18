#pragma once

struct InputData final {
	bool P1Up;
	bool P1Down;
	bool P2Up;
	bool P2Down;
	bool Reset;
};

// data structure:
//	0		0		0		1		1		1		1		1
//							reset	p1up	p1down	p2up	p2down
static InputData byteToInputData(char byte) {
	InputData data;

	data.P1Up	= (byte & 1 << 0);
	data.P1Down = (byte & 1 << 1);
	data.P2Up   = (byte & 1 << 2);
	data.P2Down = (byte & 1 << 3);
	data.Reset  = (byte & 1 << 4);

	return data;
}
#pragma once

#include <Arduino.h>

class DS1868Driver
{
private:
	byte trim1=128, trim2=128;
	byte rstPin, clkPin, datPin;

protected:
	void transmitByte(byte value);
	void transmitBit(byte bit);

public:
	DS1868Driver();
	~DS1868Driver();
	void begin(byte rstPin, byte clkPin, byte datPin);
	void setTrims();
	inline void setTrimer1Value(byte value) { trim1 = value; };
	inline void setTrimer2Value(byte value) { trim2 = value; };
};

extern DS1868Driver ds1868;

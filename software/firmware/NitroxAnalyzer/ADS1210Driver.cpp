#include "ADS1210Driver.h"


ADS1210Driver::ADS1210Driver(unsigned long freqIn)
{
	ADS1210Driver::frequencyIn = freqIn;
}

void ADS1210Driver::begin(byte clk, byte arduinoOutADS1210In, byte arduionInADS1210Out, CommandRegister& comreg) {
	din_pin = arduionInADS1210Out;
	dout_pin = arduinoOutADS1210In;
	clk_pin = clk;

	pinMode(din_pin, INPUT_PULLUP);
	pinMode(dout_pin, OUTPUT);
	pinMode(clk_pin, OUTPUT);

	digitalWrite(clk_pin, LOW);
	digitalWrite(dout_pin, LOW);

	//setup chip configuration 
	insreg.bits.rw = WRITE;
	insreg.bits.mb = _4BYTES;
	insreg.bits.address = COMMAND_REGISTER_BYTE_3_MSB;

	//transmit instruction byte
	transmitByte(insreg.insr);

	//transmitting command byte
	for (int i = 3; i >= 0; i--) {
		transmitByte(comreg.data[i]);
	}

	Serial.print(F("Sent configuration to ads1210 : ("));
	for (int i = 3; i >= 0; i--) {
		Serial.print(comreg.data[i]);
		if (i>0)
			Serial.print(F(")-("));
		else
			Serial.print(F(")"));
	}

	Serial.println();


	//Now Read the configuration

	CommandRegister cr;

	//Readcommand
	insreg.bits.rw = READ;
	insreg.bits.mb = _4BYTES;
	insreg.bits.address = COMMAND_REGISTER_BYTE_3_MSB;

	transmitByte(insreg.insr);

	if (DEBUG)
		Serial.print("Receiving Data : ");

	for (int i = 3; i >= 0; i--) {
		cr.data[i] = receiveByte();
	}

	if (DEBUG)
		Serial.println();

	if ((cr.data[3] & 0xFE) != (comreg.data[3] & 0xFE)) {
		Serial.println(F("We have a very big problem :::::: cannot get or write right creg values from ads1210."));
	}
}

void ADS1210Driver::begin(byte clk, byte arduinoOutADS1210In, byte arduionInADS1210Out) {
	//defaults
	creg.bias = Off;
	creg.referenceOutput = On;
	creg.dataFormat = TwosComplement;
	creg.unipolar = Unipolar;
	creg.byteOrder = MostSignificantByteFirst;
	creg.bitOrder = MostSignificantBitFirst;
	creg.serialDataLine = SDOUT;
	creg.operationMode = BackgroundCalibration;
	creg.pgaMode = GAIN_16;
	creg.channel = Channel1;
	creg.turboMode = TurboModeRate1;

	//calculate decimation rate 5 times per second
	calculateAndSetDecimationRate(100, creg);

	begin(clk, arduinoOutADS1210In, arduionInADS1210Out, creg);
}

ADS1210Driver::~ADS1210Driver()
{

}

void ADS1210Driver::calculateAndSetDecimationRate(int fData, CommandRegister& comreg) {
	double tmr;

	//Cannot be faster
	if (fData > 1000)
		fData = 1000;

	if (DEBUG) {
		char buffer[64];
		if (DEBUG)
			sprintf(buffer, "frequencyIn, turboMode,fdata -> %lu, %i, %i\\0", frequencyIn, comreg.turboMode, fData);
		Serial.println(buffer);
	}

	double decimationRatio = (((double)frequencyIn * pow(2, (double)comreg.turboMode)) / ((double)fData*512.0)) - 1.0;

	if (DEBUG) {
		Serial.print(F("Calculated DecimationRation for data frequency "));
		Serial.print(fData);
		Serial.print(F(" = "));
		Serial.println(decimationRatio);
	}

	//cannot be greater
	if (decimationRatio >= 8000)
		decimationRatio = 7999;

	//cannot be smaller
	if (decimationRatio < 19)
		decimationRatio = 19;

	comreg.decimationRatio = (int)decimationRatio;
}

boolean ADS1210Driver::isPGAControlCorrectBasedOnTurboModeRate() {
	return pga2trm(creg.pgaMode, creg.turboMode);
}

boolean ADS1210Driver::pga2trm(int a, int b) {
	switch (a) {
	case 0:
		return true;
	case 1:
		if (b <= 8)
			return true;
		break;
	case 2:
		if (b <= 4)
			return true;
		break;
	case 3:
		if (b <= 2)
			return true;
		break;
	case 4:
		if (b == 1)
			return true;
		break;
	}

	return false;
}

boolean ADS1210Driver::isTurboModeRateCorrectBasedOnPGAControl(){
	return pga2trm(creg.turboMode, creg.pgaMode);
}

boolean ADS1210Driver::setTurboMode(ComReg_TurboModeRate tmr){
	if (isTurboModeRateCorrectBasedOnPGAControl()) {
		creg.turboMode = tmr;
	}
}

boolean ADS1210Driver::setGain(ComReg_GainSetting gain){
	if (isPGAControlCorrectBasedOnTurboModeRate()) {
		creg.pgaMode = gain;
	}
}

/* Returns the long value of the ADC output*/
long ADS1210Driver::getDigitalOutputValue () {
	long v = ADS1210.registers.dor.bytes.do2;
	v = v << 8;
	v += ADS1210.registers.dor.bytes.do1;
	v = v << 8;
	v += ADS1210.registers.dor.bytes.do0;

	return v;
}

/*
Reads the content of the Data Output Register. creg Bit and Byte order are supported.
*/

/*
long ADS1210Driver::getDigitalOutputValue() {
	long data = 0;
	unsigned long l = 0;
	boolean msb = (creg.byteOrder == MostSignificantByteFirst);

	//set isnreg stuff and sendit
	insreg.bits.rw = READ;
	insreg.bits.mb = _3BYTES;
	insreg.bits.address = DATA_OUTPUT_REG_BYTE_2_MSB;

	//Normally called just after a DRDY low detection
	delayMicroseconds(2); // min 5.5 * 100 nS => 550 ns  then 1 Us should be enough

	//write insr 
	transmitByte(insreg.insr);
	delayMicroseconds(20);

	for (int i = (msb ? insreg.bits.mb : 0); (msb ? i >= 0 : i <= insreg.bits.mb); (msb ? i-- : i++)) {
		//get byte
		l = receiveByte();
		data += (l << (8 * i));

		//set data sign if twos complement
		if (creg.dataFormat == TwosComplement) {
			if (msb) {
				if (insreg.bits.mb == i && insreg.bits.mb<3)
					data += ((l & 0x80) << 24);
			}
		}
	}

	if (DEBUG) {
		Serial.print("s{");
		Serial.print(l);
		Serial.print(", ");
		Serial.print(data);
		Serial.println("}");
	}

	return data;
}*/

void ADS1210Driver::readRegisters() {
	long data = 0;
	unsigned long l = 0;
	boolean msb = (creg.byteOrder == MostSignificantByteFirst);

	//set isnreg stuff and sendit
	insreg.bits.rw = READ;
	insreg.bits.mb = _3BYTES;
	insreg.bits.address = DATA_OUTPUT_REG_BYTE_2_MSB;

	//Normally called just after a DRDY low detection
	delayMicroseconds(2); // min 5.5 * 100 nS => 550 ns  then 1 Us should be enough

	//write insr 
	transmitByte(insreg.insr);
	delayMicroseconds(5);

	for (int i = (msb ? insreg.bits.mb : 0); (msb ? i >= 0 : i <= insreg.bits.mb); (msb ? i-- : i++)) {
		//get byte
		l = receiveByte();
		registers.dor.data[i] = l;
	}

	//set isnreg stuff and sendit
	insreg.bits.rw = READ;
	insreg.bits.mb = _4BYTES;
	insreg.bits.address = COMMAND_REGISTER_BYTE_3_MSB;

	//Normally called just after a DRDY low detection
	delayMicroseconds(2); // min 5.5 * 100 nS => 550 ns  then 1 Us should be enough

	//write insr 
	transmitByte(insreg.insr);
	delayMicroseconds(5);

	for (int i = (msb ? insreg.bits.mb : 0); (msb ? i >= 0 : i <= insreg.bits.mb); (msb ? i-- : i++)) {
		//get byte
		l = receiveByte();
		registers.cmr.data[i] = l;
	}

	//set isnreg stuff and sendit
	insreg.bits.rw = READ;
	insreg.bits.mb = _3BYTES;
	insreg.bits.address = OFFSET_CAL_REGISTER_BYTE_2_MSB;

	//Normally called just after a DRDY low detection
	delayMicroseconds(2); // min 5.5 * 100 nS => 550 ns  then 1 Us should be enough

	//write insr 
	transmitByte(insreg.insr);
	delayMicroseconds(5);

	for (int i = (msb ? insreg.bits.mb : 0); (msb ? i >= 0 : i <= insreg.bits.mb); (msb ? i-- : i++)) {
		//get byte
		l = receiveByte();
		registers.ocr.data[i] = l;
	}

	//set isnreg stuff and sendit
	insreg.bits.rw = READ;
	insreg.bits.mb = _3BYTES;
	insreg.bits.address = FULL_SCALE_CAL_REG_BYTE_2_MSB;

	//Normally called just after a DRDY low detection
	delayMicroseconds(2); // min 5.5 * 100 nS => 550 ns  then 1 Us should be enough

	//write insr 
	transmitByte(insreg.insr);
	delayMicroseconds(5);

	for (int i = (msb ? insreg.bits.mb : 0); (msb ? i >= 0 : i <= insreg.bits.mb); (msb ? i-- : i++)) {
		//get byte
		l = receiveByte();
		registers.fcr.data[i] = l;
	}
}

/*
Send a bit to the ADS1210 IC
*/
void ADS1210Driver::transmitBit(byte bit) {
	digitalWrite(clk_pin, HIGH);
	digitalWrite(dout_pin, bit);
	_NOP();
	digitalWrite(clk_pin, LOW);
	_NOP();
}

/*
Sends a byte to the ADS1210 IC
*/
void ADS1210Driver::transmitByte(byte value) {
	byte bit;

	//set clock to low
	digitalWrite(clk_pin, LOW);

	for (int i = 7; i >= 0; i--) {
		bit = value;
		bit &= (1 << i);
		bit >>= i;
		transmitBit(bit);
	}
}

/*
Receives a bit from the ADS1210 IC
*/
byte ADS1210Driver::receiveBit() {
	byte b;
	digitalWrite(clk_pin, HIGH);
//	delayMicroseconds(1);
	_NOP();
	digitalWrite(clk_pin, LOW);
	b = digitalRead(din_pin);
	//delayMicroseconds(1);
	_NOP();

	if (DEBUG)
		Serial.print(b);

	return b;
}

/*
Receives one byte from the ADS1210 IC
*/
byte ADS1210Driver::receiveByte() {
	byte b = 0;
	byte received = 0;
	boolean msb = creg.bitOrder == MostSignificantBitFirst;

	//set clock to low
	digitalWrite(clk_pin, LOW);

	if (DEBUG)
		Serial.print(" {");

	for (int i = (msb ? 7 : 0); (msb ? i >= 0 : i<=0); (msb? i--: i++)) {
		received = receiveBit();
		//Serial.print(received);
		b += (received << i);
	}

	if (DEBUG) {
		Serial.print(" = ");
		Serial.print(b);
		Serial.print("}");
	}

	return b;
}

//10MHz = the resonator frequency
ADS1210Driver ADS1210 = ADS1210Driver(10000000);
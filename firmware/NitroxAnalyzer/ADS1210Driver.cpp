#include "ADS1210Driver.h"


ADS1210Driver::ADS1210Driver(unsigned long freqIn)
{
	ADS1210Driver::frequencyIn = freqIn;
}

void ADS1210Driver::begin(byte clk, byte arduinoOutADS1210In, byte arduionInADS1210Out) {
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
	
	/*
	Serial.print(F("INSREG = "));
	Serial.println(insreg.insr);
	*/

	//defaults
	creg.bias = Off;
	creg.referenceOutput = On;
	creg.dataFormat = TwosComplement;
	creg.unipolar = Bipolar;
	creg.byteOrder = MostSignificantByteFirst;
	creg.bitOrder = MostSignificantBitFirst;
	creg.serialDataLine = SDOUT;
	creg.operationMode = BackgroundCalibration;
	creg.pgaMode = GAIN_2;
	creg.channel = Channel1;
	creg.turboMode = TurboModeRate16;

	/* Works as expected 
	==============================================
	Serial.print(F("CREG[3] = "));
	Serial.println(creg.cmr[3]);
	Serial.print(F("CREG[2] = "));
	Serial.println(creg.cmr[2]);
	Serial.print(F("CREG[1] = "));
	Serial.println(creg.cmr[1]);
	Serial.print(F("CREG[0] = "));
	Serial.println(creg.cmr[0]);
	*/
	//calculate decimation rate 5 times per second
	calculateAndSetDecimationRate(50);

	//Write command
	//should set dio_pin as output
	//pinMode(dio_pin, OUTPUT);

	//transmit instruction byte
	transmitByte(insreg.insr);

	//transmitting command byte
	for (int i = 3; i >= 0; i--) {
		transmitByte(creg.cmr[i]);
	}

	Serial.print(F("Sent configuration to ads1210 : ("));
	for (int i = 3; i >= 0; i--) {
		Serial.print(creg.cmr[i]);
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
		cr.cmr[i] = receiveByte();
	}

	if (DEBUG)
		Serial.println();

	if ((cr.cmr[3] & 0xFE) != (creg.cmr[3]&0xFE)) {
		Serial.println(F("We have a very big problem :::::: cannot get or write right creg values from ads1210."));
	}
}

ADS1210Driver::~ADS1210Driver()
{

}

void ADS1210Driver::calculateAndSetDecimationRate(int fData) {
	double tmr;

	//Cannot be faster
	if (fData > 1000)
		fData = 1000;

	if (DEBUG) {
		char buffer[64];
		if (DEBUG)
			sprintf(buffer, "frequencyIn, turboMode,fdata -> %lu, %i, %i\\0", frequencyIn, creg.turboMode, fData) ;
		Serial.println(buffer);
	}

	double decimationRatio = (((double)frequencyIn * pow(2,(double)creg.turboMode)) / ((double)fData*512.0)) - 1.0;

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

	creg.decimationRatio = (int)decimationRatio;
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

/*
Reads the content of the Data Output Register. creg Bit and Byte order are supported.
*/
long ADS1210Driver::readDataOutputRegister() {
	long data = 0;
	unsigned long l;
	boolean msb = creg.byteOrder == MostSignificantByteFirst;

	//set isnreg stuff and sendit
	insreg.bits.rw = READ;
	insreg.bits.mb = _3BYTES;
	insreg.bits.address = DATA_OUTPUT_REG_BYTE_2_MSB;

	//Normally called just after a DRDY low detection
	delayMicroseconds(1); // min 5.5 * 100 nS => 550 ns  then 1 Us should be enough

	//write insr 
	transmitByte(insreg.insr);
	delayMicroseconds(10);

	for (int i = (msb ? insreg.bits.mb : 0); (msb ? i >= 0 : i <= insreg.bits.mb); (msb? i--: i++)) {
		//get byte
		l = receiveByte();
		data += (l << (8 * i));
		
		//set data sign if twos complement
		if (creg.dataFormat == TwosComplement ) {
			if (msb) {
				if (insreg.bits.mb == i && insreg.bits.mb<3)
					data += ((l & 0x80) << 24);
			}
			else{

			}
		}

		if (DEBUG) {
			Serial.print("{");
			Serial.print(l);
			Serial.print(", ");
			Serial.print(data);
			Serial.println("}");
		}
	}
	
	return data;
}

/*
Send a bit to the ADS1210 IC
*/
void ADS1210Driver::transmitBit(byte bit) {
	digitalWrite(clk_pin, HIGH);
	digitalWrite(dout_pin, bit);
	delayMicroseconds(1);
	digitalWrite(clk_pin, LOW);
	delayMicroseconds(1);
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
	delayMicroseconds(1);
	digitalWrite(clk_pin, LOW);
	b = digitalRead(din_pin);
	delayMicroseconds(1);

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
ADS1210Driver ads1210 = ADS1210Driver(10000000);
#pragma once

#include <Arduino.h>

#define DEBUG true

typedef enum ReadWrite{
	WRITE = 0,
	READ
} InstructionReg_RW;

typedef enum  MultiByte{
	_1BYTE = 0,
	_2BYTES,
	_3BYTES,
	_4BYTES
} InstructionReg_MB;

typedef enum CommandAddress{
	DATA_OUTPUT_REG_BYTE_2_MSB = 0,
	DATA_OUTPUT_REG_BYTE_1,
	DATA_OUTPUT_REG_BYTE_0_LSB,

	COMMAND_REGISTER_BYTE_3_MSB = 4,
	COMMAND_REGISTER_BYTE_2,
	COMMAND_REGISTER_BYTE_1,
	COMMAND_REGISTER_BYTE_0_LSB,

	OFFSET_CAL_REGISTER_BYTE_2_MSB = 8,
	OFFSET_CAL_REGISTER_BYTE_1,
	OFFSET_CAL_REGISTER_BYTE_0_LSB,

	FULL_SCALE_CAL_REG_BYTE_2_MSB = 12,
	FULL_SCALE_CAL_REG_BYTE_1,
	FULL_SCALE_CAL_REG_BYTE_0_LSB,
} InstructionReg_CommandAddress;

typedef union InstructionRegister{
	byte insr;
	struct {
		unsigned address : 4; //LSBit
		unsigned : 1;
		unsigned mb : 2;
		unsigned rw : 1; //MSBit
	} bits;
} InstructionReg;


typedef enum OnOff{
	Off = 0,
	On
} ComReg_Bias, ComReg_ReferenceOutput;

typedef enum DataFormat{
	TwosComplement = 0,
	OffsetBinary
} ComReg_DataFormat;

typedef enum Polarity{
	Bipolar = 0,
	Unipolar
} ComReg_Unipolar;

typedef enum  BytOrder{
	MostSignificantByteFirst = 0,
	LeastSignificantByteFirst
} ComReg_ByteOrder;

typedef enum BitOrder{
	MostSignificantBitFirst = 0,
	LeastSignificantBitFirst
} ComReg_BitOrder;

typedef enum SerialDataOutputPin{
	SDIO = 0, 
	SDOUT
} ComReg_SerialDataOutputPin;

typedef enum DataReady{
	DataReady = 0,
	DataNotReady
} ComReg_DataReady;

typedef enum OperatingMode{
	NormalMode = 0,
	SelfCalibration,
	SystemOffsetCalibration,
	SystemFullScaleCalibration,
	PseudoSystemCalibration,
	BackgroundCalibration,
	Sleep,
	Reserved
} ComReg_OperatingMode;

typedef enum GainSetting{
	GAIN_1 = 0,
	GAIN_2,
	GAIN_4,
	GAIN_8,
	GAIN_16
} ComReg_GainSetting;

typedef enum  Channels{
	Channel1 = 0,
	Channel2,
	Channel3,
	Channel4
} ComReg_Channels;

typedef enum TurboModeRate{
	TurboModeRate1 = 0,
	TurboModeRate2,
	TurboModeRate4,
	TurboModeRate8,
	TurboModeRate16
} ComReg_TurboModeRate;


typedef union CommandRegister{
	byte cmr[4];
	struct {
		unsigned decimationRatio : 13; // LSByte and LSBit
		unsigned turboMode : 3;	// default turbo mode rate = 1				
		unsigned channel : 2;	// default 00 Channel 1
		unsigned pgaMode : 3;	// default 000 Gain 1
		unsigned operationMode : 3;	// default 000 Normal Mode	
		unsigned dataReady : 1;	// no default
		unsigned serialDataLine : 1;	// default SDIO	
		unsigned bitOrder : 1;	// defualt MSbit first
		unsigned byteOrder : 1;	// default MSB first MostSignificantByteFirst
		unsigned unipolar : 1;	// default bipolar	
		unsigned dataFormat : 1;	// default off => Two's complement
		unsigned referenceOutput : 1;	// default on
		unsigned bias : 1;	// default off				MSBYTE and MSBit
	};
} CommandRegister;


class ADS1210Driver
{
public:
	ADS1210Driver(unsigned long ads1210Frequency);
	~ADS1210Driver();
	void begin(byte clk, byte dIn, byte dOut);
	long readDataOutputRegister();

protected:
	void calculateAndSetDecimationRate(int fData);
	boolean isPGAControlCorrectBasedOnTurboModeRate();
	boolean isTurboModeRateCorrectBasedOnPGAControl();
	void transmitByte(byte value);
	void transmitBit(byte value);
	byte receiveByte();
	byte receiveBit();

private:
	unsigned long frequencyIn;
	byte clk_pin;
	byte din_pin;
	byte dout_pin;

	void setDecimationRate();
	boolean pga2trm(int a, int b);

	boolean setTurboMode(ComReg_TurboModeRate tmr);
	boolean setGain(ComReg_GainSetting gain);

	CommandRegister creg;
	InstructionReg insreg;
};

extern ADS1210Driver ads1210;


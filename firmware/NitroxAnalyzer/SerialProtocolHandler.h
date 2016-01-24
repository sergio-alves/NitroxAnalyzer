#pragma once

#include <Arduino.h>
#include "DS1868Driver.h"
#include <Stream.h>

#include "SerialCommand.h"
#include "GetAverageCommand.h"
#include "SetO2CellValidityDateCommand.h"
#include "GetBatteryLoadCommand.h"
#include "GetO2CellInstallDateCommand.h"
#include "SetO2CellInstallDateCommand.h"

#define AVAILABLE_COMMANDS_COUNT	5
#define IN_BUFFER_SIZE				64
#define OUT_BUFFER_SIZE				64

/* The class specialized in Nitrox Analyzer serial communication */
class SerialProtocolHandler
{
public:
	/* Constructs a protocol handler by passing a Stream from which chars are read or written to */
	SerialProtocolHandler(Stream& serialStream);

	/* Returns the next command parsed if any or NULL */
	SerialCommand * getNextCommand();

	/* Sends a response to the Serial command received */
	void sendResponse(SerialCommand * c);

	/* Sends an error */
	void sendErrorResponse(int id, const PROGMEM char * str);

protected:
	/* Read an append a byte to the input buffer */
	void getSerialData();

	/* Checks through all existing commands if one match returns NULL pointer otherwise */
	SerialCommand * parseLine();

private:
	SerialCommand * cmdList[AVAILABLE_COMMANDS_COUNT];
	Stream& serial;

	byte inBuffer[IN_BUFFER_SIZE];
	byte * inBufferPos = inBuffer;
	byte outBuffer[OUT_BUFFER_SIZE];

	/* Clean and reset input buffer */
	void cleanAndResetInBuffer();
};

extern SerialProtocolHandler protocolHandler;

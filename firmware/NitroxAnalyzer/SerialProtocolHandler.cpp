#include "SerialProtocolHandler.h"

/* Constructs a protocol handler by passing a Stream from which chars are read or written to */
SerialProtocolHandler::SerialProtocolHandler(Stream& serialStream) : serial(serialStream){
	//Build the list of available commands
	cmdList[0] = new GetAverageCommand();
	cmdList[1] = new SetO2CellValidityDateCommand();
	cmdList[2] = new GetBatteryLoadCommand();
	cmdList[3] = new GetO2CellInstallDateCommand();
	cmdList[4] = new SetO2CellInstallDateCommand();
}

/* Returns the next command parsed if any or NULL */
SerialCommand * SerialProtocolHandler::getNextCommand() {
	//Get last byte (do it byty by byte)
	getSerialData();
	return parseLine();
}

/* Checks through all existing commands if one match returns NULL pointer otherwise */
SerialCommand * SerialProtocolHandler::parseLine() {
	//read line till C char
	int var;

	//All commands starts with GET/SET <OBJECT>
	for (int i = 0; i < AVAILABLE_COMMANDS_COUNT; i++){
		if (cmdList[i]->parse(inBuffer, IN_BUFFER_SIZE)) {
			cleanAndResetInBuffer();
			return cmdList[i];
		}
	}

	if (strchr((const char*)inBuffer, 10) != NULL) {
		sendErrorResponse(0, (const char *)F("No valid command"));
		cleanAndResetInBuffer();
	}

	return NULL;
}

/* Clean and reset input buffer */
void SerialProtocolHandler::cleanAndResetInBuffer() {
	Serial.println(F("Cleaning input buffer"));
	memset(inBuffer, 0, 64);
	inBufferPos = inBuffer;
}

/* Sends a response to the Serial command received */
void SerialProtocolHandler::sendResponse(SerialCommand * c) {
	c->createResponse(outBuffer, 64);
	serial.println((char *)outBuffer);
}

/* Sends an error response */
void SerialProtocolHandler::sendErrorResponse(int id, const char * str) {
	memset(outBuffer, 0, OUT_BUFFER_SIZE);
	sprintf_P((char *)outBuffer, (const char *)F("ERROR [%i] %s"), id, str);
	serial.println((char *)outBuffer);
}

/* Read an append a byte to the input buffer */
void SerialProtocolHandler::getSerialData() {
	if (serial.available() > 0) {
		if ((inBufferPos - inBuffer) < 64) {
			*inBufferPos = (byte)serial.read();
			inBufferPos++;
		}
	}
}

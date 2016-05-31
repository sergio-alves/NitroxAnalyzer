#pragma once

#include <Arduino.h>

/* Enumeration of available serial commands*/
enum SerialCommandEnum {
	GET_AVERAGE = 1,
	GET_BATTERY_LOAD,
	GET_O2_CELL_INSTALL_DATE,
	GET_O2_CELL_VALIDITY_DATE,
	SET_O2_CELL_INSTALL_DATE,
	SET_O2_CELL_VALIDITY_DATE,
	GET_ACKNOWLEDGE,
};

/* The representation of a serial command used between Nitrox Analyzer and Android app*/
class SerialCommand
{
public:
	/* Parses the input buffer looking for this command string */
	virtual boolean parse(char * buffer, int size) = 0;

	/* Builds a response using command properties */
	virtual void createResponse(char * buffer, int size) = 0;
	
	/* Returns an id corresponding to this command */
	virtual SerialCommandEnum getCommandID() = 0 ;

	/* Returns this command name */
	virtual const PROGMEM char * getName() = 0;
};

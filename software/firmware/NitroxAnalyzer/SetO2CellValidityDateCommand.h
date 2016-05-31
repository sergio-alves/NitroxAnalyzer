#pragma once

#include "SerialCommand.h"

/* Representation of the SET O2 CELL VALIDITY DATE Serial command */
class SetO2CellValidityDateCommand: public SerialCommand 
{
public:
	/* Parses the input buffer looking for this command string */
	boolean parse(char * buffer, int size);
	
	/* Builds a response using command properties */
	void createResponse(char * buffer, int size);
	
	/* Returns an id corresponding to this command */
	SerialCommandEnum getCommandID() { return SET_O2_CELL_VALIDITY_DATE;}

	/* Returns present command name */
	inline const PROGMEM char * getName() { return (const char *)F("Set O2 Cell Validity Date"); }

private:
	int day;
	int month;
	int year;
};


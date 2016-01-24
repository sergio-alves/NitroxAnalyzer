#pragma once

#include "SerialCommand.h"

/* Representation of the SET O2 CELL Install DATE Serial command */
class SetO2CellInstallDateCommand : public SerialCommand
{
public:
	/* Parses the input buffer looking for this command string */
	boolean parse(byte * buffer, int size);

	/* Builds a response using command properties */
	void createResponse(byte * buffer, int size);

	/* Returns an id corresponding to this command */
	SerialCommandEnum getCommandID() { return SET_O2_CELL_INSTALL_DATE; }

	/* Returns present command name */
	inline const PROGMEM char * getName() { return (const char *)F("Set O2 Cell Install Date"); }

	/* Get the install day */
	inline int getDay() { return day; }

	/* Get the install month */
	inline int getMonth() { return month; }

	/* Get the install year */
	inline int getYear() { return year; }

private:
	int day;
	int month;
	int year;
};

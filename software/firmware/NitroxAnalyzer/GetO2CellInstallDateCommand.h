#pragma once

#include "SerialCommand.h"

/* The O2 Cell Install Date serial command representation */
class GetO2CellInstallDateCommand: public SerialCommand
{
public:
	/* Parses the input buffer looking for this command string */
	boolean parse(char * buffer, int size);

	/* Builds a response using command properties */
	void createResponse(char * buffer, int size);

	/* Returns an id corresponding to this command */
	inline SerialCommandEnum getCommandID() { return GET_O2_CELL_INSTALL_DATE; }

	/* Returns this command name */
	inline const PROGMEM char * getName() { (const char *)F("Get O2 Cell Install Date"); }

	/* sets the percent of battery load */
	inline void setDate(int d, int m, int y) { day = d; month = m; year = y; }

private:
	int day;
	int month;
	int year;
};
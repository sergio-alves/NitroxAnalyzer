#pragma once

#include "SerialCommand.h"

/* The battery load serial command representation */
class GetBatteryLoadCommand : public SerialCommand
{
public:
	/* Parses the input buffer looking for this command string */
	boolean parse(byte * buffer, int size);

	/* Builds a response using command properties */
	void createResponse(byte * buffer, int size);

	/* Returns an id corresponding to this command */
	inline SerialCommandEnum getCommandID() { return GET_BATTERY_LOAD; }

	/* Returns this command name */
	inline const PROGMEM char * getName() { (const char *)F("Get Battery Load"); }

	/* sets the percent of battery load */
	inline void setLoad(unsigned int l) { load = l; }

private:
	unsigned int load;
};
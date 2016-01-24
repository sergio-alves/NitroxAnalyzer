#pragma once

#include "SerialCommand.h"

/* Representation of the GET AVERAGE Serial command */
class GetAverageCommand : public SerialCommand
{
public:
	/* Parses the input buffer looking for this command string */
	boolean parse(byte * buffer, int size);
	void createResponse(byte * buffer, int size);

	/* Returns an id corresponding to this command */
	inline SerialCommandEnum getCommandID() { return GET_AVERAGE;}

	/* Returns this command name */
	inline const PROGMEM char * getName() { return (const char *)F("Get Average"); }

	/* Returns the number of iterations */
	inline int getIterations() { return iterations; }

	/* Sets the calculated average for given number of read iterations */
	inline void setAverage(unsigned long avg) { average = avg; }

private:
	unsigned long average;
	int iterations;
};


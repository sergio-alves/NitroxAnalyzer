#include "SetO2CellValidityDateCommand.h"
/* Parses the input buffer looking for this command string */
boolean SetO2CellValidityDateCommand::parse(byte * buffer, int size) {
	//set a bound to the buffer 
	buffer[size - 1] = 0;
	char * _b = (char *)buffer;

	if (strchr(_b, 10) != NULL) {
		if (sscanf_P(_b, (const char *)F("SET O2 CELL VALIDITY DATE %i-%i-%i"), &day, &month, &year) == 3) {
			return true;
		}
	}

	return false;
}

/* Builds a response using command properties */
void SetO2CellValidityDateCommand::createResponse(byte * buffer, int size) {
	memset(buffer, 0, size);
	char * _b = (char *)buffer;
	sprintf_P(_b, (char *)F("RET O2 CELL VALIDITY SET"));
}
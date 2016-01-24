#include "GetO2CellInstallDateCommand.h"

/* Parses the input buffer looking for this command string */
boolean GetO2CellInstallDateCommand::parse(byte * buffer, int size) {
	//set a bound to the buffer 
	buffer[size - 1] = 0;
	char * _b = (char *)buffer;

	if (strchr(_b, 10) != NULL) {
		if (strstr_P(_b, (const char *)F("GET O2 CELL INSTALL DATE")) != NULL) {
			return true;
		}
	}

	return false;
}

/* Builds a response using command properties */
void GetO2CellInstallDateCommand::createResponse(byte * buffer, int size) {
	memset(buffer, 0, size);
	char * _b = (char *)buffer;
	sprintf_P(_b, (const char*)F("RET O2 CELL INSTALL DATE %u-%u-%u"), day, month, year);
}
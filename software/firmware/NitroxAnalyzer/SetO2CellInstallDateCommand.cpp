#include "SetO2CellInstallDateCommand.h"

/* Parses the input buffer looking for this command string */
boolean SetO2CellInstallDateCommand::parse(char * buffer, int size) {
	//set a bound to the buffer 
	buffer[size - 1] = 0;
	char * _b = buffer;

	if (strchr(_b, 10) != NULL) {
		if (sscanf_P(_b, (char *)F("SET O2 CELL INSTALL DATE %i-%i-%i"), &day, &month, &year) == 3) {
			return true;
		}
	}

	return false;
}

/* Builds a response using command properties */
void SetO2CellInstallDateCommand::createResponse(char * buffer, int size) {
	memset(buffer, 0, size);
	sprintf(buffer, "RET O2 CELL INSTALL DATE SET");
}

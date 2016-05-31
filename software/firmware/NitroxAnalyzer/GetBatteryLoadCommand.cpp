#include "GetBatteryLoadCommand.h"

/* Parses the input buffer looking for this command string */
boolean GetBatteryLoadCommand::parse(char * buffer, int size) {
	//set a bound to the buffer 
	buffer[size - 1] = 0;
	char * _b = buffer;

	if (strchr(_b, 10) != NULL) {
		if (strstr_P(_b, (const char *)F("GET BATTERY LOAD")) != NULL) {
			return true;
		}
	}

	return false;
}

/* Builds a response using command properties */
void GetBatteryLoadCommand::createResponse(char * buffer, int size) {
	memset(buffer, 0, size);
	char * _b = (char *)buffer;
	sprintf_P(_b, (const char*)F("RET BATTERY LOAD %u"), load);
}
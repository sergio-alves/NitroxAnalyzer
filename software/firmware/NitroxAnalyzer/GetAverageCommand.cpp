#include "GetAverageCommand.h"

/* Parses the input buffer looking for this command string */
boolean GetAverageCommand::parse(char * buffer, int size) {
	//set a bound to the buffer 
	buffer[size - 1] = 0;
	char * _b = buffer;

	if (strchr(_b, 10) != NULL) {
		if (sscanf_P(_b, (const char*)F("GET AVERAGE %i"), &iterations) == 1) {
			return true;
		}
	}

	return false;
}

/* Builds a response using command properties */
void GetAverageCommand::createResponse(char * buffer, int size) {
	memset(buffer, 0, size);
	char * _b = (char *)buffer;
	sprintf_P(_b, (const char*)F("RET AVERAGE %lu"), average);
}
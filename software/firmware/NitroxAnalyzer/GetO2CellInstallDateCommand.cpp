#include "GetO2CellInstallDateCommand.h"

/* Parses the input buffer looking for this command string */
boolean GetO2CellInstallDateCommand::parse(char * buffer, int size) {
	//set a bound to the buffer 
	buffer[size - 1] = 0;
	return strchr(buffer, 10) != NULL && strstr_P(buffer, (const char *)F("GET O2 CELL INSTALL DATE")) != NULL;
}

/* Builds a response using command properties */
void GetO2CellInstallDateCommand::createResponse(char * buffer, int size) {
	memset(buffer, 0, size);
	int d = day;
	int m = month;
	int y = year;
	sprintf(buffer, "RET O2 CELL INSTALL DATE %02d-%02d-%4d", d,m,y);
	Serial.println(buffer);
}
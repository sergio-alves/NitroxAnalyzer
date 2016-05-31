#include "OzOledDisplayAdapter.h"

OzOledDisplayAdapter::OzOledDisplayAdapter() : ozOled(OzOled){

}

void OzOledDisplayAdapter::init()
{
	ozOled.init();
}

void OzOledDisplayAdapter::setSegmentRemap(int remapValue) {
	ozOled.sendCommand(remapValue);
}

/**
* Displays the splash screen and move to next state (delay before display the second screen)
*/
void OzOledDisplayAdapter::displaySplashScreen() {
	Serial.println(F("Displaying splash Screen"));
	ozOled.clearDisplay();               // clear the screen and set start position to top left corner
	ozOled.drawBitmap(welcomeScreen, 2, 0, 128, 64);
}

/* Display the a screen to ask user to turn valv on*/
void OzOledDisplayAdapter::displayTurnValvOnScreen() {
	clearDisplay(0, 4, 0, 128);
	ozOled.printString("Please, turn", 2, 0, 12);
	ozOled.printString("valve on", 4, 1, 8);
}

void OzOledDisplayAdapter::displayFlowIndicatorScreen(){
	clearDisplay(0, 4, 0, 128);
	ozOled.printString("Flow indicator", 1, 0, 14);

	ozOled.printString("more", 1, 1, 4);

	//Draws the center
	ozOled.drawBitmap(&center[2], 63, 16, (int)pgm_read_byte(&center[0]), (int)pgm_read_byte(&center[1]));

	ozOled.printString("less", 11, 1, 4);
}

void OzOledDisplayAdapter::updateFlowIndicator(int flowMappedValue) {
	//Clears display
	clearDisplay(4, 5, 0, 128);

	//Draws the center
	ozOled.drawBitmap(&center[2], 63, 32, (int)pgm_read_byte(&center[0]), (int)pgm_read_byte(&center[1]));


	//draws the indicator for the flow  
	if (flowMappedValue < 0){
		for (int i = flowMappedValue; i < 0; i++) {
			ozOled.drawBitmap(&left[2], 63 + i* (int)pgm_read_byte(&left[0]), 32, (int)pgm_read_byte(&left[0]), (int)pgm_read_byte(&left[1]));
		}
	}
	else if (flowMappedValue > 0) {
		for (int i = 0; i < flowMappedValue; i++) {
			ozOled.drawBitmap(&right[2], 65 + i * (int)pgm_read_byte(&right[0]), 32, (int)pgm_read_byte(&right[0]), (int)pgm_read_byte(&right[1]));
		}
	}
	else {
		//perfect flow
		clearDisplay(0, 5, 0, 128);
	}
}

/**
* Displays the small battery status by reading the battery remaining voltage.
* Battery status moves from full -> 75% -> 50% -> 25% ->nearly empty
*/
void OzOledDisplayAdapter::drawBatteryIcon(int col, int page, int batteryStatus) {
	/*
}
void OzOledDisplayAdapter::displayBatteryStatus(int batteryStatus, int x, int y) {
	*/

	clearDisplay(page, page + 1, col, col + 16);

	switch (batteryStatus) {
	default:
	case 0:
		drawBitmap(col, page, bat0Percent);
		break;
	case 1:
		drawBitmap(col, page, bat25Percent);
		break;
	case 2:
		drawBitmap(col, page, bat50Percent);
		break;
	case 3:
		drawBitmap(col, page, bat75Percent);
		break;
	case 4:
		drawBitmap(col, page, bat100Percent);
		break;
	}
}

void OzOledDisplayAdapter::clearDisplay() {
	ozOled.clearDisplay();
}

/**
* Displays the main interface screen and after that set state to idle  aka state = 1000
*/
void OzOledDisplayAdapter::displayMainScreen(int batteryStatus) {
	Serial.println(F("Displaying main interface"));
	Serial.print(F("Battery Status : "));
	Serial.println(batteryStatus);
	ozOled.clearDisplay();               // clear the screen and set start position to top left corner
	drawBitmap(8, 0, startMenu);
	drawBitmap(88, 0, calibMenu);
	drawBatteryIcon(56, 0, batteryStatus);
}

void OzOledDisplayAdapter::drawBitmap(int col, int page, const byte * raw) {
	ozOled.drawBitmap(&raw[2], col, page*8, (int)pgm_read_byte(&raw[0]), (int)pgm_read_byte(&raw[1]));
}

void OzOledDisplayAdapter::drawDataInOutIcon(int col, int page, int state) {
	switch (state) {
		case 0:
			drawBitmap(col, page, data_in_out_void);
			break;
		case 1:
			drawBitmap(col, page, data_in);		
			break;
		case 2:
			drawBitmap(col, page, data_out);		
			break;
		case 3:
			drawBitmap(col, page, data_in_out);
			break;
	}
}

void OzOledDisplayAdapter::drawBluetoothIconState(int col, int page, int state) {
	switch(state) {
		case 0:
			drawBitmap(col, page, bt_searching_0);
			break;
		case 1:
			drawBitmap(col, page, bt_searching_1);
			break;
		case 2:
			drawBitmap(col, page, bt_searching_2);
			break;
		case 3:
			drawBitmap(col, page, bt_searching_3);
			break;
	}
}

/* from o to 100 %*/
void OzOledDisplayAdapter::drawProgressBar(int col, int page, int progress) {
	int elements = map(progress, 0, 100, 0, 30);

	for (int i = 0; i < elements; i++) {
		drawBitmap(4 + i * (int)pgm_read_byte(&progressElement[0]), page, progressElement);
	}
}

/**
* Clears display (override OzOled function by clearing by column and not by character)
*/
void OzOledDisplayAdapter::clearDisplay(int spage, int epage, int scol, int ecol)  {
	byte mp = 0, column = 0;

	for (mp = spage; mp <= epage; mp++) {

		ozOled.sendCommand(0x00 + (scol & 0x0F));     //set column lower address
		ozOled.sendCommand(0x10 + ((scol >> 4) & 0x0F)); //set column higher address
		ozOled.sendCommand(0xB0 + mp);//set page address

		for (column = scol; column <= ecol; column++) { //clear all columns
			ozOled.sendData(0);
		}
	}

	ozOled.sendCommand(0x00 + (76 & 0x0F));     //set column lower address
	ozOled.sendCommand(0x10 + ((76 >> 4) & 0x0F)); //set column higher address
	ozOled.sendCommand(0xB0 + mp);//set page address
}

void OzOledDisplayAdapter::drawText(int col, int page, const char * text) {
	byte len = strlen(text);

	if (col == (int)horizontalTextAlignmentCenter) { // center alignement
		ozOled.printString(text, (16-len)/2, page, len);
	} else {
		ozOled.printString(text, col/8, page, len);
	}
}

/**
* Update the status bar. progress variable value used.
*/
/*void OzOledDisplayAdapter::progressBarUpdate(int progress) {
	ozOled.drawBitmap(&progressElement[2], 4 + ((int)(progress * 0.28)) * (int)pgm_read_byte(&progressElement[0]), 40, (int)pgm_read_byte(&progressElement[0]), (int)pgm_read_byte(&progressElement[1]));

	Serial.print(F("Repainting element @ (x,y): ("));
	Serial.print(4 + ((int)(progress * 0.28)) * (int)pgm_read_byte(&progressElement[0]));
	Serial.println(F(", 40)"));
}*/

void OzOledDisplayAdapter::drawOxygenRate(int col, int page, byte digs[]) {
	//defines the start position (right most boundary)
	int pos = 128-col;
	
	clearDisplay(page, page + 1, 0, 128);
	
	for (int i = 7; i >= 0; i--) {
		//Serial.print(F("Drawind digit : "));
		//Serial.println(digs[i]);
		pos -= (int)pgm_read_byte(&digits[digs[i]][0]);
		drawBitmap(pos, page, digits[digs[i]]);
	}
}

void OzOledDisplayAdapter::displayCurrentO2AnalogValueAndMv(int analog, double mv){
	char sbuffer[16];
	int mvpre = (int)(mv * 1000.0);
	int mvpost = (int)(((mv * 1000) - mvpre) * 100);
	sbuffer[15] = 0;
	sprintf(sbuffer, "%04i %i.%2i mv\0", analog, mvpre, mvpost);

	ozOled.printString(sbuffer, 1, 5, 15);
}

OzOledDisplayAdapter ozOledDisplayAdapter;


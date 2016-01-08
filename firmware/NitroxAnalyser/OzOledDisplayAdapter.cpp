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
void OzOledDisplayAdapter::displayBatteryStatus(int batteryStatus) {

  clearDisplay(5, 6, 56, 72);
  
	switch (batteryStatus) {
	default:
	case 0:
		ozOled.drawBitmap(&bat0Percent[2], 56, 48, (int)pgm_read_byte(&bat0Percent[0]), (int)pgm_read_byte(&bat0Percent[1]));
		break;
	case 1:
		ozOled.drawBitmap(&bat25Percent[2], 56, 48, (int)pgm_read_byte(&bat25Percent[0]), (int)pgm_read_byte(&bat25Percent[1]));
		break;
	case 2:
		ozOled.drawBitmap(&bat50Percent[2], 56, 48, (int)pgm_read_byte(&bat50Percent[0]), (int)pgm_read_byte(&bat50Percent[1]));
		break;
	case 3:
		ozOled.drawBitmap(&bat75Percent[2], 56, 48, (int)pgm_read_byte(&bat75Percent[0]), (int)pgm_read_byte(&bat75Percent[1]));
		break;
	case 4:
		ozOled.drawBitmap(&bat100Percent[2], 56, 48, (int)pgm_read_byte(&bat100Percent[0]), (int)pgm_read_byte(&bat100Percent[1]));
		break;
	}
}

/**
* Displays the main interface screen and after that set state to idle  aka state = 1000
*/
void OzOledDisplayAdapter::displayMainScreen(int batteryStatus) {
	Serial.println(F("Displaying main interface"));
  Serial.print(F("Battery Status : "));
  Serial.println(batteryStatus);
	ozOled.clearDisplay();               // clear the screen and set start position to top left corner
	ozOled.drawBitmap(&startMenu[2], 8, 48, (int)pgm_read_byte(&startMenu[0]), (int)pgm_read_byte(&startMenu[1]));
	ozOled.drawBitmap(&calibMenu[2], 88, 48, (int)pgm_read_byte(&calibMenu[0]), (int)pgm_read_byte(&calibMenu[1]));
	displayBatteryStatus(batteryStatus);
}

/**
* Clears display (override OzOled function by clearing by column and not by character)
*/
void OzOledDisplayAdapter::clearDisplay(int spage, int epage, int scol, int ecol)  {
	byte mp = 0, column = 0;

	for (mp = spage; mp < epage; mp++) {

		ozOled.sendCommand(0x00 + (scol & 0x0F));     //set column lower address
		ozOled.sendCommand(0x10 + ((scol >> 4) & 0x0F)); //set column higher address
		ozOled.sendCommand(0xB0 + mp);//set page address

		for (column = scol; column < ecol; column++) { //clear all columns
			ozOled.sendData(0);
		}
	}

	ozOled.sendCommand(0x00 + (76 & 0x0F));     //set column lower address
	ozOled.sendCommand(0x10 + ((76 >> 4) & 0x0F)); //set column higher address
	ozOled.sendCommand(0xB0 + mp);//set page address
}

/**
* Update the status bar. progress variable value used.
*/
void OzOledDisplayAdapter::progressBarUpdate(int progress) {
	ozOled.drawBitmap(&progressElement[2], 4 + ((int)(progress * 0.28)) * (int)pgm_read_byte(&progressElement[0]), 40, (int)pgm_read_byte(&progressElement[0]), (int)pgm_read_byte(&progressElement[1]));

	Serial.print(F("Repainting element @ (x,y): ("));
	Serial.print(4 + ((int)(progress * 0.28)) * (int)pgm_read_byte(&progressElement[0]));
	Serial.println(F(", 40)"));
}

void OzOledDisplayAdapter::displayOxygenRate(byte digs[]) {
	//defines the start position (right most boundary)
	int pos = 120;
	for (int i = 7; i >= 0; i--) {
		Serial.print(F("Drawind digit : "));
		Serial.println(digs[i]);
		pos -= (int)pgm_read_byte(&digits[digs[i]][0]);
		ozOled.drawBitmap(&(digits[digs[i]][2]), pos, 0, (int)pgm_read_byte(&digits[digs[i]][0]), (int)pgm_read_byte(&digits[digs[i]][1]));
	}
}

void OzOledDisplayAdapter::displayCurrentO2AnalogValueAndMv(int analog, double mv){
	char sbuffer[16];
	int mvpre = (int)(mv * 1000.0);
	int mvpost = (int)(((mv*1000) - mvpre) * 100);
	sbuffer[15] = 0;
	sprintf(sbuffer, "%04i %i.%2i mv\0", analog, mvpre, mvpost);
	
	ozOled.printString(sbuffer, 1, 5, 15);
}

OzOledDisplayAdapter ozOledDisplayAdapter;



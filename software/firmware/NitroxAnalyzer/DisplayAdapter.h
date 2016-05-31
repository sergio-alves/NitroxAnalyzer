#pragma once

#include <Arduino.h>
#include "Constants.h"

class DisplayAdapter {
public:
	virtual void displaySplashScreen() = 0;
	virtual void displayMainScreen(int batteryStatus) = 0;
	virtual void displayTurnValvOnScreen() = 0;
	virtual void displayFlowIndicatorScreen() = 0;
	virtual void updateFlowIndicator(int flowMappedValue) = 0;

	virtual void drawText(int col, int page, const char * text) = 0;
	virtual void drawBatteryIcon(int, int, int) = 0;
	virtual void drawBitmap(int, int, const byte*) = 0;
	virtual void drawProgressBar(int col, int page, int progress) = 0;
	virtual void clearDisplay(int spage, int epage, int scol, int ecol) = 0;
	virtual void drawOxygenRate(int col, int page, byte digs[]) = 0;
	virtual void setSegmentRemap(int remapValue);
	virtual void init() = 0;
	virtual void displayCurrentO2AnalogValueAndMv(int analog, double mv) = 0;
};

extern DisplayAdapter &displayAdapter;
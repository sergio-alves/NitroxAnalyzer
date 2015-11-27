#ifndef _DISPLAYADAPTER_H
#define _DISPLAYADAPTER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "DisplayAdapter.h"
#include "Constants.h"

class DisplayAdapter {
public:
	virtual void displaySplashScreen() = 0;
	virtual void displayMainScreen(int batteryStatus) = 0;
	virtual void displayTurnValvOnScreen() = 0;
	virtual void displayFlowIndicatorScreen() = 0;
	virtual void updateFlowIndicator(int flowMappedValue) = 0;

	virtual void displayBatteryStatus(int batteryStatus) = 0;
	virtual void progressBarUpdate(int progress) = 0;
	virtual void clearDisplay(int spage, int epage, int scol, int ecol) = 0;
	virtual void displayOxygenRate(byte digs[]) = 0;
	virtual void setSegmentRemap(int remapValue);
	virtual void init() = 0;
};

extern DisplayAdapter &displayAdapter;

#endif //_DISPLAYADAPTER_H

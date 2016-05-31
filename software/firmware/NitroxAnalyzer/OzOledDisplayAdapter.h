#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <OzOLED.h>
#include "DisplayAdapter.h"

enum horizontalTextAlignment {
	horizontalTextAlignmentCenter = 1023,
	horizontalTextAlignmentLeft = 1024,
	horizontalTextAlignmentRight = 1025,
};

class OzOledDisplayAdapter : public DisplayAdapter
{
private:
	OzOLED &ozOled;

protected:

public:
	OzOledDisplayAdapter();

	void init();
	void displaySplashScreen();
	void displayMainScreen(int batteryStatus);



	

	void displayTurnValvOnScreen();
	void displayFlowIndicatorScreen();
	void updateFlowIndicator(int flowMappedValue);
	void printEnableNitroxFlow();

	void setSegmentRemap(int remapValue);
	void displayCurrentO2AnalogValueAndMv(int analog, double mv);
	void clearDisplay();
	void clearDisplay(int spage, int epage, int scol, int ecol);
	void drawBitmap(int col, int page, const byte * raw);
	void drawBatteryIcon(int col, int page, int batteryStatus);
	void drawProgressBar(int col, int page, int progress);
	void drawText(int col, int page, const char * text);
	void drawOxygenRate(int col, int page, byte digs[]);
	void drawBluetoothIconState(int col, int page, int state);
	void drawDataInOutIcon(int col, int page, int state);
};

extern OzOledDisplayAdapter ozOledDisplayAdapter;

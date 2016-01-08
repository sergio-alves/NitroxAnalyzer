// OzOledDisplayAdapter.h

#ifndef _I2COLEDDISPLAYADAPTER_h
#define _I2COLEDDISPLAYADAPTER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <OzOLED.h>
#include "DisplayAdapter.h"

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
	void displayBatteryStatus(int batteryStatus);
	void progressBarUpdate(int progress);
	
	void clearDisplay(int spage, int epage, int scol, int ecol);
	void displayOxygenRate(byte digs[]);

	void displayTurnValvOnScreen();
	void displayFlowIndicatorScreen();
	void updateFlowIndicator(int flowMappedValue);
	void printEnableNitroxFlow();

	void setSegmentRemap(int remapValue);
	void displayCurrentO2AnalogValueAndMv(int analog, double mv);

};

extern OzOledDisplayAdapter ozOledDisplayAdapter;

#endif //_I2COLEDDISPLAYADAPTER_h

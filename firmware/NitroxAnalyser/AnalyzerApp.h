#ifndef __ANALYZER_APP_H__
#define __ANALYZER_APP_H__

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <eeprom.h>
#include "Hardware.h"
#include "DisplayAdapter.h"
#include "RingBuffer.h"

typedef struct {
	byte currentState;
	unsigned long lastStateChange;
} ButtonState;

typedef union {
	byte data[8];
	double coeff;
} LinearCoefficient;

class AnalyzerApp {

public:
	AnalyzerApp(DisplayAdapter &);
	void doLoop();
	void doSetup();

private:
	void checkAndDebounce(int index, int pinId, void (AnalyzerApp::*)());
	void analyzeTaskBlinkingText();
	void analyzeTaskDisplayMessageToEnableNitroxFlow();

	void actionOnStartButtonClick();
	void actionOnCalibrationButtonClick();
	boolean nonBlockingDelayElapsed(int ms);
	boolean analyzeTask();
	boolean calibrationTask();
	void analyzeTaskDoesFlowExists();

	void analyzeTaskReadValue(const __FlashStringHelper* task, int currentStep, int nextStep, bool calibrate);
	void analyzeTaskFadeProgressBar(const __FlashStringHelper* task, int currentStep, int nextStep);
	void printTransitionMessage(const __FlashStringHelper* task, int currentStep, int nextStep);
	void displayValue(int decval, bool update);
	long fromIntToDigitsArray(int digit, long decvalcp, long power);
	void analyzeTaskFlowRegulation();
	byte sign(int value);
	void calibrate(double value);
	double getAnalogValueAverage();


	// private variables
	int state = 0;

	ButtonState states[2];

	unsigned long start = 0;
	int anaSteps = 0;
	int blinkcnt = 0;

	long mappedValue = 0;
	boolean numDigitFound = false;
	byte digs[8];
	byte progress = 0;
	byte oldprogress = 0;
	byte gencnt = 0;
  
	int max = 0, min = 1023;
	char buffer[128];
	char sbuffer[16];

	void debug();

	DisplayAdapter &displayAdapter;

	LinearCoefficient linear_coeff;

	RingBuffer rb ; //initializes a ringbuffer with 10 elements
};

extern AnalyzerApp app;

#endif //__ANALYZER_APP_H__


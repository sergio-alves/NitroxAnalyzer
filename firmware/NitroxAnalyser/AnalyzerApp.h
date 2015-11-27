#ifndef __ANALYZER_APP_H__
#define __ANALYZER_APP_H__

#include <Arduino.h>
#include "Hardware.h"
#include "DisplayAdapter.h"

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

	void analyzeTaskReadValue(const __FlashStringHelper* task, int currentStep, int nextStep);
	void analyzeTaskFadeProgressBar(const __FlashStringHelper* task, int currentStep, int nextStep);
	void printTransitionMessage(const __FlashStringHelper* task, int currentStep, int nextStep);
	void displayValue(int decval);
	long fromIntToDigitsArray(int digit, long decvalcp, long power);
	void analyzeTaskFlowRegulation();
	byte sign(int value);

	// private variables
	int state = 0;
	byte buttonsOldValues[2] = { 1, 1 };
	unsigned long buttonLastStateChangeMillis[2] = { millis(), millis() };

	unsigned long start = 0;
	int anaSteps = 0;
	int blinkcnt = 0;

	long mappedValue = 0;
	boolean numDigitFound = false;
	byte digs[8];
	byte progress = 0;
	byte oldprogress = 0;
	byte gencnt = 0;

	DisplayAdapter &displayAdapter;
};

extern AnalyzerApp app;

#endif //__ANALYZER_APP_H__


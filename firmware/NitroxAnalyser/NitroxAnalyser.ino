#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <OzOLED.h>

#include "AnalyzerApp.h"
#include "OzOledDisplayAdapter.h"

/********************************************************************************************************************************************/
AnalyzerApp app = AnalyzerApp(ozOledDisplayAdapter);

/**
 * The arduino setup method
 */
void setup() {
  app.doSetup();
}

/**
 * The main arduino loop
 */
void loop() {
  app.doLoop();
}


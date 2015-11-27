#include <HardwareSerial.h>
#include <Wire.h>
#include <OzOLED.h>
//#include "Hardware.h"
//#include "Constants.h"
#include "DisplayAdapter.h"
#include "AnalyzerApp.h"

/********************************************************************************************************************************************/
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


#include <Arduino.h>
#include <Stream.h>
#include <stdio.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Wire.h>
#include <OzOLED.h>
#include "AnalyzerApp.h"


/* This app setup*/
void setup()
{
	app.doSetup();
}

void loop() {
	app.doLoop();
}

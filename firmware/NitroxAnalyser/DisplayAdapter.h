#ifndef __DISPLAY_ADAPTER_H__
#define __DISPLAY_ADAPTER_H__

#include <Arduino.h>
#include <Wire.h>
#include <OzOLED.h>
#include "DisplayAdapter.h"
#include "Constants.h"

class DisplayAdapter: public OzOLED{
    public :
      void displaySplashScreen();
      void displayMainScreen(int batteryStatus) ;
      void displayBatteryStatus(int batteryStatus);
      void progressBarUpdate(int progress);
      void clearDisplay(int spage, int epage, int scol, int ecol);  
      void displayOxygenRate(byte digs[]);
            
      //to override
      inline void drawBitmap(const byte *bitmaparray, byte x, byte y, byte width, byte height){ OzOLED::drawBitmap(bitmaparray, x, y, width, height);};
      inline void printString(const char *str, byte x=255, byte y=255, byte numChar=255){OzOLED::printString(str, x,y,numChar);};
      inline void sendCommand(byte command){OzOLED::sendCommand(command);};
      inline void sendData(byte data){OzOLED::sendData(data);};
      inline void init() {OzOLED::init();};
};

extern DisplayAdapter displayAdapter;

#endif //__DISPLAY_ADAPTER_H__

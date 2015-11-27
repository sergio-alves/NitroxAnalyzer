#include "DisplayAdapter.h"


/**
 * Displays the splash screen and move to next state (delay before display the second screen)
 */
void DisplayAdapter::displaySplashScreen() {
  Serial.println(F("Displaying splash Screen"));
  OzOLED::clearDisplay();               // clear the screen and set start position to top left corner
  drawBitmap(welcomeScreen, 2, 0, 128, 64);
}


/**
 * Displays the small battery status by reading the battery remaining voltage.
 * Battery status moves from full -> 75% -> 50% -> 25% ->nearly empty 
 */
void DisplayAdapter::displayBatteryStatus(int batteryStatus) {
  switch (batteryStatus) {
    default:
    case 0:
      drawBitmap(&bat0Percent[2], 56, 48, (int)pgm_read_byte(&bat0Percent[0]), (int)pgm_read_byte(&bat0Percent[1]));
      break;
    case 1:
      drawBitmap(&bat25Percent[2], 56, 48, (int)pgm_read_byte(&bat25Percent[0]), (int)pgm_read_byte(&bat25Percent[1]));
      break;
    case 2:
      drawBitmap(&bat50Percent[2], 56, 48, (int)pgm_read_byte(&bat50Percent[0]), (int)pgm_read_byte(&bat50Percent[1]));
      break;
    case 3:
      drawBitmap(&bat75Percent[2], 56, 48, (int)pgm_read_byte(&bat75Percent[0]), (int)pgm_read_byte(&bat75Percent[1]));
      break;
    case 4:
      drawBitmap(&bat100Percent[2], 56, 48, (int)pgm_read_byte(&bat100Percent[0]), (int)pgm_read_byte(&bat100Percent[1]));
      break;
  }  
}

/**
 * Displays the main interface screen and after that set state to idle  aka state = 1000
 */
void DisplayAdapter::displayMainScreen(int batteryStatus) {
  Serial.println(F("Displaying main interface"));
  OzOLED::clearDisplay();               // clear the screen and set start position to top left corner
  drawBitmap(&startMenu[2], 8, 48, (int)pgm_read_byte(&startMenu[0]), (int)pgm_read_byte(&startMenu[1]));
  drawBitmap(&calibMenu[2], 88, 48, (int)pgm_read_byte(&calibMenu[0]), (int)pgm_read_byte(&calibMenu[1]));
  displayBatteryStatus(batteryStatus);
}

/**
 * Clears display (override OzOled function by clearing by column and not by character)
 */
void DisplayAdapter::clearDisplay(int spage, int epage, int scol, int ecol)  {
  byte mp = 0, column = 0;

  for (mp = spage; mp < epage; mp++) {

    sendCommand(0x00 + (scol & 0x0F));     //set column lower address
    sendCommand(0x10 + ((scol >> 4) & 0x0F)); //set column higher address
    sendCommand(0xB0 + mp);//set page address

    for (column = scol; column < ecol; column++) { //clear all columns
      sendData(0);
    }
  }

  sendCommand(0x00 + (76 & 0x0F));     //set column lower address
  sendCommand(0x10 + ((76 >> 4) & 0x0F)); //set column higher address
  sendCommand(0xB0 + mp);//set page address
}

/**
 * Update the status bar. progress variable value used.
 */
void DisplayAdapter::progressBarUpdate(int progress) {
    drawBitmap(&progressElement[2], 4 + ((int)(progress * 0.28)) * (int)pgm_read_byte(&progressElement[0]), 40, (int)pgm_read_byte(&progressElement[0]), (int)pgm_read_byte(&progressElement[1]));

    Serial.print(F("Repainting element @ (x,y): ("));
    Serial.print(4 + ((int)(progress * 0.28)) * (int)pgm_read_byte(&progressElement[0]));
    Serial.println(F(", 40)"));
}

void DisplayAdapter::displayOxygenRate(byte digs[]) {
  //defines the start position (right most boundary)
  int pos = 120;
  for (int i = 7; i >= 0; i--) {
    Serial.print(F("Drawind digit : "));
    Serial.println(digs[i]);    
    pos -= (int)pgm_read_byte(&digits[digs[i]][0]);
    displayAdapter.drawBitmap(&(digits[digs[i]][2]), pos, 0, (int)pgm_read_byte(&digits[digs[i]][0]), (int)pgm_read_byte(&digits[digs[i]][1]));
  }  
}

DisplayAdapter displayAdapter;

#include "AnalyzerApp.h"

void AnalyzerApp::doSetup() {
  Serial.begin(115200);
  displayAdapter.init();                       // initialze Oscar OLED display
  displayAdapter.sendCommand(0xA0);            // should be placed inside displayadapter
  delay(100);
  state = 0;
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);
}

void AnalyzerApp::doLoop() {
  //Check start analyse button
  checkAndDebounce (0, LEFT_BUTTON_PIN, &AnalyzerApp::actionOnStartButtonClick);
  
  //Check calibrate button
  checkAndDebounce (1, RIGHT_BUTTON_PIN, &AnalyzerApp::actionOnCalibrationButtonClick);

  //Software main state loop
  switch (state) {
    case 0:
      displayAdapter.displaySplashScreen();
      Serial.println(F("Changing state : 0 -> 1"));
      state = 1;
      start = millis();      
      break;
    case 1:
      if(nonBlockingDelayElapsed(500)) {
        state = 2;
        Serial.println(F("Changing state : 1 -> 2"));
      }
      break;
    case 2:
      displayAdapter.displayMainScreen(map(analogRead(BATTERY_ANALOG), 800, 1023, 0, 4));    
      Serial.println(F("Changing state : 2 -> idle"));
      state = 1000;
      break;
    case 3: 
      if(analyzeTask()){
        Serial.println(F("Changing state : 3 -> Idle"));
        state = 1000;
      }
      break;
    case 4: //Start Calibration
      if(calibrationTask()){
        Serial.println(F("Changing state : 4 -> Idle"));   
        state = 1000;     
      }
      break;      
    default:
      break;
  }
}

/**
 * Checks button and debounce
 */
void AnalyzerApp::checkAndDebounce(int index, int pinId, void (AnalyzerApp::*f)()) {
  if (buttonsOldValues[index] != digitalRead(pinId) && (millis() - buttonLastStateChangeMillis[index]) > 100) {
    buttonLastStateChangeMillis[index] = millis();

    //Change occured let's handle
    if (buttonsOldValues[index] == 1) {
      //Falling edge => must do job
      (app.*f)();
    }

    buttonsOldValues[index] = digitalRead(pinId);
  }
}

/**
 * Sets netxt state to analyze state
 */
void AnalyzerApp::actionOnStartButtonClick() {
  Serial.println(F("Start analyse button pressed"));
  state = 3;
}

/**
 * Sets next main state to calibration state
 */
void AnalyzerApp::actionOnCalibrationButtonClick() {
  Serial.println(F("Start Calibration button pressed"));
  state = 4;
}


/**
 * A non blocking delay. 
 * if delay elapsed returns true else false
 */
boolean AnalyzerApp::nonBlockingDelayElapsed(int ms) {
  if ((millis() - start) > ms) {
    return true;
  }
  return false;
}



/**
 * The analyze workflow 
 */
boolean AnalyzerApp::analyzeTask() {
  switch(anaSteps) {
    case 0:
      analyzeTaskDoesFlowExists();
      break;
    case 1: 
      analyzeTaskDisplayMessageToEnableNitroxFlow();           
      break;      
    case 2: 
      analyzeTaskBlinkingText();
      break;    
    case 3:
      displayAdapter.clearDisplay(0, 4, 0, 128);
      OzOled.printString("Flow indicator", 1, 0, 14); 
      OzOled.printString("more   |  less", 1, 1, 14); 
      start = millis();
      Serial.println(F("Analyzer step 3 -> 4"));
      anaSteps = 4;
    case 4:
      analyzeTaskFlowRegulation();      
      break;
    case 5:
      analyzeTaskReadValue(F("Analyzer"), 5, 6);
      break;
    case 6:
      analyzeTaskFadeProgressBar(F("Analyzer"),6, 7);
      break;      
    case 7:
      //Before leave... clean state machine variable anaSteps and set main loop to idle
      anaSteps = 0;
      Serial.println(F("Fadding finished"));
      return true;
  }
  return false;
}

/**
 * Calibration workflow
 */
boolean AnalyzerApp::calibrationTask() {
  switch(anaSteps) {
    case 0:
      analyzeTaskReadValue(F("Calibration"), 0, 1);
      break;
    case 1:
      analyzeTaskFadeProgressBar(F("Calibration"), 1, 2);
      break;      
    case 2:
      //Before leave... clean state machine variable anaSteps and set main loop to idle
      anaSteps = 0;
      Serial.println(F("Fadding finished"));
      return true;
  }  
  return false;
}

/**
 * Does flow exists ? 
 */
void AnalyzerApp::analyzeTaskDoesFlowExists() {
  //read air circulation sensor (motor with propeler)
  if(analogRead(FLOW_DETECTOR_ANALOG) >  50) {
    //flow exists
    anaSteps = 3;
    Serial.println(F("Analyzer step 0 -> 3"));
  } else {
    //flow doesn't exist
    anaSteps = 1;
    Serial.println(F("Analyzer step 0 -> 1"));        
  }        
}

/**
 * Prints some text
 */
void AnalyzerApp::printEnableNitroxFlow() {
  displayAdapter.clearDisplay(0, 4, 0, 128);
  displayAdapter.printString("Enable nitrox", 1, 0, 13);
  displayAdapter.printString("flow", 6, 1, 4);  
}

/**
 * Displays the information message
 */
void AnalyzerApp::analyzeTaskDisplayMessageToEnableNitroxFlow() {
  printEnableNitroxFlow();
  start = millis();
  anaSteps = 2;
  blinkcnt = 0;
  Serial.println(F("Analyzer step 1 -> 2"));   
}

/**
 * Blinks the information message and clear the screen at the end
 */
void AnalyzerApp::analyzeTaskBlinkingText() {
  switch(blinkcnt) {
    case 0: //wait 1.5s and clear
      if(millis() - start > 1500) {
        displayAdapter.clearDisplay(0,2,0,128);                              
        start = millis();
        blinkcnt++;
      }        
      break;
    case 1: //wait 0.5s and display
    case 3:
    case 5:
      if(millis()-start > 500) {
        printEnableNitroxFlow();
        start = millis();
        blinkcnt++;
      }
      break;
    case 2: //wait 0.5 and clear
    case 4:
    case 6:  
      if(millis() - start > 500) {
        displayAdapter.clearDisplay(0,2,0,128);                              
        start = millis();
        blinkcnt++;
      }    
      break;
    case 7:                                 
      if(millis() - start > 500) {
        anaSteps = 0; //Try to detect flow again
        Serial.println(F("Analyzer step 2 -> 0"));
      }
      break;
  }  
}

/**
 * The math.sign function
 */
byte AnalyzerApp::sign(int value) {
  if(value>0)
    return 1;
  else if (value < 0)
    return -1;
  else
    return 0;
}

/**
 * Checks the flow and display a pointer to help flow regulation before analyze
 */
void AnalyzerApp::analyzeTaskFlowRegulation() {
  if(millis() - start > 500) {
    //check each 500ms 
    start = millis();
  
    //flow between 200 and 250 is perfect  
    mappedValue = map(analogRead(FLOW_DETECTOR_ANALOG), 0, 1023, 11, -11);
    
    Serial.print(F("Mapped Value : "));
    Serial.println(mappedValue);
  
    //Clears display
    displayAdapter.clearDisplay(4, 5, 0, 128);
    
    //Draws the center
    displayAdapter.drawBitmap(&center[2], 63, 32, (int)pgm_read_byte(&center[0]), (int)pgm_read_byte(&center[1]));
    
    //draws the indicator for the flow
  
    if(mappedValue < 0 ){
      for(int i = mappedValue; i < 0; i++) {
        displayAdapter.drawBitmap(&left[2], 63 + i* (int)pgm_read_byte(&left[0]), 32, (int)pgm_read_byte(&left[0]), (int)pgm_read_byte(&left[1]));
      }
    }else if (mappedValue > 0) {
      for(int i=0; i < mappedValue; i++) {
        displayAdapter.drawBitmap(&right[2], 65 + i * (int)pgm_read_byte(&right[0]), 32, (int)pgm_read_byte(&right[0]), (int)pgm_read_byte(&right[1]));
      }
    } else {
        //perfect flow
        displayAdapter.clearDisplay(0, 5, 0, 128);
        anaSteps = 5;
        Serial.println(F("Analyzer step 4 -> 5"));
        start = millis();
    }              
  }
}

/**
 * Finds the right digit
 */
long AnalyzerApp::fromIntToDigitsArray(int digit, long decvalcp, long power) {
  long tmp = (decvalcp - (decvalcp % power)) / power;

  if (!numDigitFound && tmp == 0) {
    digs[digit] = 11; //blank
  } else {
    numDigitFound = true;
    digs[digit] = tmp;
  }
  decvalcp = decvalcp - tmp * power;
  return decvalcp;
}

/**
 * Displays the Oxy percent value
 */
void AnalyzerApp::displayValue(int decval) {
  int decvalcp= decval;

  Serial.print(F("Going to display following value : "));
  Serial.print(decval);
  
  displayAdapter.clearDisplay(0, 4, 0, 128);
  
  if (decval / 10000 == 1) {
    //prints digit 100.00 at
    digs[0] = 1;
    digs[1] = 0;
    digs[2] = 0;
    digs[3] = 10;
    digs[4] = 0;
    digs[5] = 0;
  } else {
    numDigitFound = false;
        
    decvalcp = fromIntToDigitsArray(0, decvalcp, 1000);
    decvalcp = fromIntToDigitsArray(1, decvalcp, 100);

    if(digs[1]==11) {
      digs[1]=0;
      numDigitFound=true;
    }
    digs[2] = 10; // Point
    
    decvalcp = fromIntToDigitsArray(3, decvalcp, 10);
    decvalcp = fromIntToDigitsArray(4, decvalcp, 1);
  }
  
  digs[5] = 12;
  digs[6] = 11;
  digs[7] = 13;

  Serial.println(F("value decomposed"));

  displayAdapter.displayOxygenRate(digs);
}


/**
 * print the 
 */
void AnalyzerApp::printTransitionMessage(const __FlashStringHelper* task, int currentStep, int nextStep){
  Serial.print(task);
  Serial.print(F(" step "));
  Serial.print(currentStep);
  Serial.print(F(" -> "));
  Serial.println(nextStep);  
}

/**
 * Reads value and display value each 280ms => ~5 times per second during about 10s
 */
void AnalyzerApp::analyzeTaskReadValue(const __FlashStringHelper* task, int currentStep, int nextStep) {
  if(millis()-start > 280){
    start = millis();
    progress++;    

    if (progress != oldprogress) {
      Serial.print(F("Progress value : "));
      Serial.println(progress);
  
      //progres value changed. Do we need to update the display too?
      if (((int)(oldprogress * 0.28)) < ((int)(progress * 0.28))) {
        //repaint;
        displayAdapter.progressBarUpdate(progress);
      }    
      oldprogress = progress;
    }


    if(progress==100){
      Serial.println(F("Calibration completed"));      
      progress = 0;
      anaSteps = nextStep; //Fade progress bar
      printTransitionMessage(task, currentStep, nextStep);      
      start = millis();      
    }else{
      if(progress % 10 == 0) {        
        mappedValue = map(analogRead(O2_CELL_ANALOG), 0, 1023, 0, 10000);
                
        //Do stuff related with task completion ... example display the value ;)
        displayValue(mappedValue); 
        
        Serial.print(F("O2 mapped value :"));
        Serial.println(mappedValue);        
      }      
    }
  }
}

/**
 * Fade the progress bar
 */
void AnalyzerApp::analyzeTaskFadeProgressBar(const __FlashStringHelper* task, int currentStep, int nextStep) {
  if (millis() - start > 100) {
    if (gencnt < 15) {
      Serial.println(F("Fadding"));
  
      displayAdapter.clearDisplay(5, 6, 8, 8 + gencnt * 4);
      displayAdapter.clearDisplay(5, 6, 120 - gencnt * 4, 120);
      gencnt++;
      start = millis();
    } else {
      //Before leave... clean state machine variable anaSteps and set main loop to idle
      gencnt = 0;
      anaSteps = nextStep;
      printTransitionMessage(task, currentStep, nextStep);
    }
  }  
}

AnalyzerApp app;


#include "AnalyzerApp.h"

//Create an instance with A displayAdapter as parameter
AnalyzerApp::AnalyzerApp(DisplayAdapter &displayAdapter) : displayAdapter(displayAdapter){
	for (int i = 0; i < 2; i++) {
		states[i].currentState = 1;
		states[i].lastStateChange = millis();
	}
}

void AnalyzerApp::doSetup() {
	Serial.begin(115200);
	displayAdapter.init();						   // initialze Oscar OLED display
	displayAdapter.setSegmentRemap(0xA0);            // should be placed inside displayadapter
	delay(100);
	state = 0;
	pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
	pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);

	bool zero = true;

	for (int i = 0; i < 8; i++) {
		linear_coeff.data[i] = EEPROM.read(i);
		if (linear_coeff.data[i] != 0) {
			
			zero = false;
		}
	}
	if (!zero) {
		Serial.print(F("Loaded linear coefficient from eeprom x * 10000 = ")); 
		Serial.println(((unsigned int)(linear_coeff.coeff * 10000.0)));
	}

	analogReference(INTERNAL);
	//START ANALOG TO FORCE REFERENCE CHANGE
	analogRead(O2_CELL_ANALOG);
	_delay_ms(10);
	/*
	anaSteps = 0;
	while (!calibrationTask()) { delay(280); }
	*/
	start = millis();
}

void AnalyzerApp::debug() {
	
	int mvpre, mvpost, minmvpre, minmvpost, maxmvpre, maxmvpost;
	int o2pre, o2post, o2minpre,o2minpost,o2maxpre,o2maxpost;

	//CHANGE REFERENCE
	int o2 = analogRead(O2_CELL_ANALOG);
	double mv = (o2 * 0.0010752688172043010752688172043);
	if (max < o2)
		max = o2;
	if (min > o2)
		min = o2;

	mvpre = (int)mv;
	mvpost = (int)((mv - mvpre) * 1000);
	
	o2pre = (int)(linear_coeff.coeff * o2);
	o2post = (int)((linear_coeff.coeff * o2 - o2pre) * 1000);

	o2minpre = (int)(linear_coeff.coeff * min);
	o2maxpre = (int)(linear_coeff.coeff * max);
	o2minpost = (int)((linear_coeff.coeff * min - o2minpre) * 1000);
	o2maxpost = (int)((linear_coeff.coeff * max - o2maxpre) * 1000);

	minmvpre = (int)(min * 0.0010752688172043010752688172043);
	maxmvpre = (int)(max * 0.0010752688172043010752688172043);
	minmvpost = (int)((min * 0.0010752688172043010752688172043 - minmvpre) * 1000);
	maxmvpost = (int)((max * 0.0010752688172043010752688172043 - maxmvpre) * 1000);

	sprintf(buffer, "%04i : %i.%i mv, %i.%i %%o2, with (min-max) [digital: %i - %i :: %% O2: %i.%i - %i.%i %% :: mv: %i.%i - %i.%i]", o2, mvpre, mvpost, o2pre, o2post, min, max, o2minpre, o2minpost, o2maxpre, o2maxpost, minmvpre,minmvpost, maxmvpre, maxmvpost);
	Serial.println(buffer);	

	displayAdapter.displayCurrentO2AnalogValueAndMv(o2, o2 * 0.0010752688172043010752688172043);
}


void AnalyzerApp::doLoop() {
	//Check start analyse button
	checkAndDebounce(0, LEFT_BUTTON_PIN, &AnalyzerApp::actionOnStartButtonClick);

	//Check calibrate button
	checkAndDebounce(1, RIGHT_BUTTON_PIN, &AnalyzerApp::actionOnCalibrationButtonClick);

	//Software main state loop
	switch (state) {
	case 0:
		displayAdapter.displaySplashScreen();
		Serial.println(F("Changing state : 0 -> 1"));
		state = 1;
		start = millis();
		break;
	case 1:
		if (nonBlockingDelayElapsed(5000)) {
			state = 2;
			Serial.println(F("Changing state : 1 -> 2"));
		}
		break;
	case 2:
    //Battery input signal is mapped between 0v and 3v 
    //Full battery will have 3V at BATTERY_ANALOG input and when 6v and less will have 0V 
		analogReference(DEFAULT);
		displayAdapter.displayMainScreen(map(analogRead(BATTERY_ANALOG), 0, 630, 0, 4));
		Serial.println(F("Changing state : 2 -> idle"));
		state = 1000;
    start = millis();
		break;
	case 3:
		if (analyzeTask()){
			Serial.println(F("Changing state : 3 -> Idle"));
			state = 1000;
			
			start = millis();
		}
		break;
	case 4: //Start Calibration
		if (calibrationTask()){
			Serial.println(F("Changing state : 4 -> Idle"));
			state = 1000;
      start = millis();
		}
		break;
	default:
		if(millis()-start > 2000) {
		  start = millis();
		  analogReference(DEFAULT);
		  analogRead(BATTERY_ANALOG);
		  delay(10);
		  displayAdapter.displayBatteryStatus(map(analogRead(BATTERY_ANALOG), 0, 630, 0, 4));

		  //CHANGE REFERENCE
		  analogReference(INTERNAL);
		  //START ANALOG TO FORCE REFERENCE CHANGE
		  analogRead(O2_CELL_ANALOG);
		  //WAIT SOME TIME
		  delay(10);

		  debug();
			
		  int min=1023;
		  int max=0;
		  int v = 0;

		  double average;
		  Serial.print(F("Reading 100 values : "));

		  //READ 100 TIMES ANALOG VALUE AND RETURN AN AVERAGE
		  for (int i = 0; i < 100; i++){
			  v = analogRead(O2_CELL_ANALOG);
			  if (i>0)
				  Serial.print(F(","));
			  Serial.print(v);

			  _delay_us(100);

			  average += v;

			  if (max < v)
				  max = v;
			  if (min > v)
				  min = v;
		  }

		  Serial.println();

		  average /= 100;

		  Serial.print(F("100 pondered values (min, max) ->(")); 
		  Serial.print(min);
		  Serial.print(F(", "));
		  Serial.print(max);
		  Serial.print(F(") analog -> "));
		  Serial.print((unsigned int)v);
		  Serial.print(F("."));
		  Serial.print((unsigned int)((v-(unsigned int)v)*100));
		  Serial.print(F(" :: coeff -> "));
		  Serial.print((unsigned int)linear_coeff.coeff);
		  Serial.print(F(" :: coeff * analog -> "));

		  double vtodisp = linear_coeff.coeff * rb.insert(v);
		  Serial.print((unsigned int)vtodisp);
		  vtodisp *= 100.0;
		  Serial.print(F(":: percent -> "));
		  Serial.println((unsigned int)vtodisp);

		  displayValue((unsigned int)vtodisp, true);
		}
		break;
	}
}


/**
 * Checks button and debounce
 */
void AnalyzerApp::checkAndDebounce(int index, int pinId, void (AnalyzerApp::*f)()) {
	if (states[index].currentState != digitalRead(pinId) && (millis() - states[index].lastStateChange) > 100) {
		states[index].lastStateChange = millis();

		//Change occured let's handle
		if (states[index].currentState == 1) {
			//Falling edge => must do job
			(app.*f)();
		}

		states[index].currentState = digitalRead(pinId);
	}
}

/**
 * Sets netxt state to analyze state
 */
void AnalyzerApp::actionOnStartButtonClick() {
	Serial.println(F("Start analyse button pressed"));
	anaSteps = 5;
	state = 3;
}

/**
 * Sets next main state to calibration state
 */
void AnalyzerApp::actionOnCalibrationButtonClick() {
	Serial.println(F("Start Calibration button pressed"));
	state = 4;
	anaSteps = 0;
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
	switch (anaSteps) {
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
		displayAdapter.displayFlowIndicatorScreen();
		start = millis();
		Serial.println(F("Analyzer step 3 -> 4"));
		anaSteps = 4;
	case 4:
		analyzeTaskFlowRegulation();
		break;
	case 5:
		//force the use of internal reference
		analogReference(INTERNAL);  //1.1v
		delay(500);
		anaSteps = 1;
	case 6:
		analyzeTaskReadValue(F("Analyzer"), 6, 7, false);
		break;
	case 7:
		analyzeTaskFadeProgressBar(F("Analyzer"), 7, 8);
		break;
	case 8:
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
	switch (anaSteps) {
	case 0:
		//force the use of internal reference
		analogReference(INTERNAL);  //1.1v
		analogRead(O2_CELL_ANALOG);
		delay(20);
		anaSteps = 1;
	case 1:
		analyzeTaskReadValue(F("Calibration"), 1, 2, true);
		break;
	case 2:
		analyzeTaskFadeProgressBar(F("Calibration"), 2, 3);
		break;
	case 3:
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
	if (analogRead(FLOW_DETECTOR_ANALOG) > 50) {
		//flow exists
		anaSteps = 3;
		Serial.println(F("Analyzer step 0 -> 3"));
	}
	else {
		//flow doesn't exist
		anaSteps = 1;
		Serial.println(F("Analyzer step 0 -> 1"));
	}
}

/**
 * Displays the information message
 */
void AnalyzerApp::analyzeTaskDisplayMessageToEnableNitroxFlow() {
	displayAdapter.displayTurnValvOnScreen();
	start = millis();
	anaSteps = 2;
	blinkcnt = 0;
	Serial.println(F("Analyzer step 1 -> 2"));
}

/**
 * Blinks the information message and clear the screen at the end
 */
void AnalyzerApp::analyzeTaskBlinkingText() {
	switch (blinkcnt) {
	case 0: //wait 1.5s and clear
		if (millis() - start > 1500) {
			displayAdapter.clearDisplay(0, 2, 0, 128);
			start = millis();
			blinkcnt++;
		}
		break;
	case 1: //wait 0.5s and display
	case 3:
	case 5:
		if (millis() - start > 500) {
			displayAdapter.displayTurnValvOnScreen();
			start = millis();
			blinkcnt++;
		}
		break;
	case 2: //wait 0.5 and clear
	case 4:
	case 6:
		if (millis() - start > 500) {
			displayAdapter.clearDisplay(0, 2, 0, 128);
			start = millis();
			blinkcnt++;
		}
		break;
	case 7:
		if (millis() - start > 500) {
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
	if (value > 0)
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
	if (millis() - start > 500) {
		//check each 500ms 
		start = millis();

		//flow between 200 and 250 is perfect  
		mappedValue = map(analogRead(FLOW_DETECTOR_ANALOG), 0, 1023, 11, -11);

		Serial.print(F("Mapped Value : "));
		Serial.println(mappedValue);

		displayAdapter.updateFlowIndicator(mappedValue); //.displayFlowIndicatorScreen();

		//draws the indicator for the flow  
		if (mappedValue == 0){
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
	}
	else {
		numDigitFound = true;
		digs[digit] = tmp;
	}
	decvalcp = decvalcp - tmp * power;
	return decvalcp;
}

/**
 * Displays the Oxy percent value
 */
void AnalyzerApp::displayValue(int decval, bool update=false) {
	int decvalcp = decval;

	Serial.print(F("Going to display following value : "));
	Serial.print(decval);

	if (!update) 
		displayAdapter.clearDisplay(0, 4, 0, 128);
	else
		displayAdapter.clearDisplay(0, 4, 0, 72);


	if (decval / 10000 == 1) {
		//prints digit 100.00 at
		digs[0] = 1;
		digs[1] = 0;
		digs[2] = 0;
		digs[3] = 10;
		digs[4] = 0;
		digs[5] = 0;
	}
	else {
		numDigitFound = false;

		decvalcp = fromIntToDigitsArray(0, decvalcp, 1000);
		decvalcp = fromIntToDigitsArray(1, decvalcp, 100);

		if (digs[1] == 11) {
			digs[1] = 0;
			numDigitFound = true;
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

double AnalyzerApp::getAnalogValueAverage() {
	double value = 0;

	//Calculating the average of last rb.getcount()
	Serial.print(F("Calculating the average of "));
	Serial.print(rb.getCount());
	Serial.println(F(" last analog reads"));

	for (int i = 0; i < rb.getCount(); i++) {
		value += (double)rb.getElementAt(i);

		if (i >0)
			Serial.print(F(" + "));
		Serial.print(rb.getElementAt(i));
	}

	Serial.print(F(" = "));

	//The average of rb.getcount();
	value /= (double)rb.getCount();

	Serial.print(F(" int val : "));

	Serial.println((unsigned int)value);

	return value;
}

void AnalyzerApp::calibrate(double value) {

	linear_coeff.coeff = 20.95 / value;

	//Stores teh coefficient into EEPROM
	for (int i = 0; i < 8; i++){
		EEPROM.write(i, linear_coeff.data[i]);
	}

	for (int i = 0; i < 8; i++){
		if (linear_coeff.data[i] != EEPROM.read(i)) {
			Serial.println(F("Error. Bad liner coefficient storage"));
			return;
		}
	}

	Serial.print(F("Coefficient Stored (x * 10000): "));
	Serial.println((unsigned int)(linear_coeff.coeff * 10000));
}

/**
 * Reads value and display value each 280ms => ~5 times per second during about 10s
 */
void AnalyzerApp::analyzeTaskReadValue(const __FlashStringHelper* task, int currentStep, int nextStep, bool cal) {
	if (millis() - start > 280){
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


		if (progress == 100){
			progress = 0;
			anaSteps = nextStep; //Fade progress bar
			double value = getAnalogValueAverage();

			//calibrate
			if (cal)
				calibrate(value);

			//Do stuff related with task completion ... example display the value ;)
			displayValue(((unsigned int)(value * linear_coeff.coeff * 100)), false);

			printTransitionMessage(task, currentStep, nextStep);
			start = millis();
		}
		else{
			if (progress % 2 == 1) {
				Serial.print(F("Got new O2 sensor value (ref=1.1v) : "));
				Serial.println(rb.insert(analogRead(O2_CELL_ANALOG)));

				//Do stuff related with task completion ... example display the value ;)
				//displayValue(mappedValue);

				/*
				Serial.print(F("O2 mapped value real->mapped:"));
				Serial.print(analogRead(O2_CELL_ANALOG));
				Serial.print(F("->"));
				Serial.println(mappedValue);
				*/
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
		}
		else {
			//Before leave... clean state machine variable anaSteps and set main loop to idle
			gencnt = 0;
			anaSteps = nextStep;
			printTransitionMessage(task, currentStep, nextStep);
		}
	}
}

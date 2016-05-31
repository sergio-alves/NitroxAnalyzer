#include "AnalyzerApp.h"

/* Load and save eeprom application configuration */
void AnalyzerApp::loadConfigurationFromEEPROM() {
	for (int i = 0; i < sizeof(AnalyzerConfiguration); i++) {
		eeprom.data[i] = EEPROM.read(EEPROM_CONFIG_START_ADDRESS + i);
	}

	Serial.println(F("EEPROM Conf loaded"));
	Serial.println(isEEPROMClean() ? F("EEPROM is clean") : F("EEPROM not clean"));

	if (eeprom.configuration.clean != 0xCAFEBABE) {
		eeprom.configuration.clean = 0xCAFEBABE;
		
		eeprom.configuration.O2Cell.install_date.year = 2015;
		eeprom.configuration.O2Cell.install_date.month = 12;
		eeprom.configuration.O2Cell.install_date.day = 1;
		eeprom.configuration.O2Cell.change_date.year = 2018;
		eeprom.configuration.O2Cell.change_date.month = 12;
		eeprom.configuration.O2Cell.change_date.day = 1;
	
		//Write it 
		for (int i = 0; i < sizeof(AnalyzerConfiguration); i++) {
			EEPROM.write(EEPROM_CONFIG_START_ADDRESS + i, eeprom.data[i]);
		}
	}
}


/* The application initial setup*/
void AnalyzerApp::doSetup() {
	Serial.begin(SERIAL_BAUDRATE);
	BTSerial.begin(BLUETOOTH_BAUDRATE);

	pinMode(2, INPUT_PULLUP);
	digitalWrite(2, HIGH);

	attachInterrupt(digitalPinToInterrupt(2), adcIRQ, FALLING);

	ADS1210.begin(ADS1210_CLK, ARDUINO_OUT_ADS1210_IN, ARDUINO_IN_ADS1210_OUT);

	pinMode(BATTERY_ANALOG_PIN, INPUT);

	//Load eeprom configuration
	loadConfigurationFromEEPROM();
	
	//ozOledDisplayAdapter display initialization
	ozOledDisplayAdapter.init();                       // initialze Oscar OLED display
	ozOledDisplayAdapter.setSegmentRemap(0xA0);        // should be placed inside displayadapter
	delay(100);

	/* Buttons related */	
	pinMode(LEFT_BUTTON, INPUT_PULLUP);
	pinMode(RIGHT_BUTTON, INPUT_PULLUP);
	digitalWrite(LEFT_BUTTON, HIGH);
	digitalWrite(RIGHT_BUTTON, HIGH);

	buttons[0].onPressed = &AnalyzerApp::onButtonPressed;
	buttons[1].onPressed = &AnalyzerApp::onButtonPressed;
	buttons[0].onRelease = &AnalyzerApp::onButtonRelease;
	buttons[1].onRelease = &AnalyzerApp::onButtonRelease;
	buttons[0].onPush = &AnalyzerApp::onButtonPush;
	buttons[1].onPush = &AnalyzerApp::onButtonPush;

	buttons[0].currentState = digitalRead(LEFT_BUTTON);
	buttons[0].lastUpdate = millis();
	buttons[1].currentState = digitalRead(RIGHT_BUTTON);
	buttons[1].lastUpdate = millis();

	/* Initial application point */
	applicationState = displaySplashScreen;
	currentLoop = SplashScreen;
}

/* The main application loop */
void AnalyzerApp::doLoop() {

	if (BTSerial.available() > 0) { // Check when a bluetooth connection is available.
		if (!btConnection){
			//run once...
			Serial.println(F("BT connection detected"));
		}

		btConnection = true; //Sets variable to indicate that bt connection was detected
		currentLoop = BluetoothScreen; //Change application main loop to the one handling bluetooth coms
		applicationState = idle; //Set application loop state to idle

		bluetoothState = 3; // updates the bt icon.
		ozOledDisplayAdapter.drawBluetoothIconState(64, 0, bluetoothState);

		//Starts ping service to discover when bt connection is lost
		counters[bluetoothConnectionAlive] = millis();
	}

	poolButtonsState();	// check buttons state

	switch (currentLoop) // call the right loop depending on current application screen
	{
	case ManualScreen:
		currentLoop = manualScreenLoop();
		break;
	case BluetoothScreen:
		currentLoop = bluetoothScreenLoop();
		break;
	case SplashScreen:
		currentLoop = splashScreenLoop();
		break;
	default:
		Serial.print(F("Default in doLoop command : "));
		Serial.println(currentLoop);
	}


	/*
	screenDisplayLoop();

	switch (applicationState) {
	case idle: //ApplicationStates::idle
	if (!btConnection) {
	if (autoCalibrate && calibrationValue == 0) { applicationState = calibrate;  Serial.println(F("Selecting Calibrate action")); }
	if (autoMeasure && calibrationValue != 0) { applicationState = measure; Serial.println(F("Selecting Measure action")); }
	}
	else{
	applicationState = getNextCommand(idle);
	}
	break;
	case calibrate:
	applicationState = doCalibration(calibrate, idle);
	break;
	case measure:
	applicationState = doO2Measure(measure, idle);
	break;
	case getAverage:
	break;
	case getBatteryLoad:
	((GetBatteryLoadCommand *)command)->setLoad(getBatteryPercent());
	inOutIconUpdate(true, false);
	protocolHandler.sendResponse(command);
	applicationState = idle;
	break;
	case getO2CellInstallDate:
	Serial.println(F("Getting o2 cell install date"));

	if (isEEPROMClean()) {
	((GetO2CellInstallDateCommand *)command)->setDate(eeprom.O2Cell.install_date.day, eeprom.O2Cell.install_date.month, eeprom.O2Cell.install_date.year);
	inOutIconUpdate(true, false);
	protocolHandler.sendResponse(command);

	sprintf(buffer, "returning date : %i/%i/%i", eeprom.O2Cell.install_date.day, eeprom.O2Cell.install_date.month, eeprom.O2Cell.install_date.year);
	Serial.println(buffer);
	} else {
	protocolHandler.sendErrorResponse(1, "No valid clean install date found");
	inOutIconUpdate(true,false);
	Serial.println(F("ERR - No valid clean install date found"));
	}
	applicationState = idle;
	break;
	case setO2CellInstallDate:
	eeprom.O2Cell.install_date.day = ((SetO2CellInstallDateCommand *)command)->getDay();
	eeprom.O2Cell.install_date.month = ((SetO2CellInstallDateCommand *)command)->getMonth();
	eeprom.O2Cell.install_date.year = ((SetO2CellInstallDateCommand *)command)->getYear();

	if (eeprom.clean != 0xCAFEBABE) {
	eeprom.clean = 0xCAFEBABE;
	eeprom.O2Cell.change_date.day = eeprom.O2Cell.install_date.day;
	eeprom.O2Cell.change_date.month = eeprom.O2Cell.install_date.month;
	eeprom.O2Cell.change_date.year = eeprom.O2Cell.install_date.year + 3;
	}

	//Load eeprom
	for (int i = 0; i < EEPROM_STORED_DATA_SIZE; i++) {
	EEPROM.write(EEPROM_CONFIG_START_ADDRESS + i, eeprom.data[i]);
	}

	protocolHandler.sendResponse(command);
	inOutIconUpdate(true, false);
	bluetoothApplicationStates = idle;
	break;
	case getO2CellValidityDate:
	break;
	case setO2CellValidityDate:
	break;

	case wait: //Do nothing
	default:
	break;
	}
	*/
}

/* called after button push / release. The length = time between push and release */
void AnalyzerApp::onButtonPressed(int btnId, int length) {
	Serial.print(F("Button "));
	Serial.print(btnId);
	Serial.print(F(" pressed during "));
	Serial.print(length);
	Serial.println(F(" ms"));
}

/* called when button pushed */
void AnalyzerApp::onButtonPush(int btnId) {
	Serial.print(F("Button "));
	Serial.print(btnId);
	Serial.println(F(" pushed"));
}

/* called when button released */
void AnalyzerApp::onButtonRelease(int btnId) {
	Serial.print(F("Button "));
	Serial.print(btnId);
	Serial.println(F(" released"));
}

/* debouce buttons */
bool AnalyzerApp::stateChangedDebounced(int id) {
	int dr = 1;

	if (id == 0)
		dr = digitalRead(LEFT_BUTTON);
	else if (id == 1)
		dr = digitalRead(RIGHT_BUTTON);

	return ((dr != buttons[id].currentState) && ((millis() - buttons[id].lastUpdate) > DEBOUNCE_MAX_MS));
}

/* pool the buttons state */
void AnalyzerApp::poolButtonsState(){
	int dr;

	for (int i = 0; i < 2; i++) {
		if (stateChangedDebounced(i)) {
			//State changed
			if (buttons[i].currentState == HIGH) {
				buttons[i].lastUpdate = millis();
				buttons[i].currentState = LOW;
				//button pressed		
				(this->*(buttons[i].onPush))(i);
			}
			else{
				buttons[i].currentState = HIGH;
				(this->*(buttons[i].onRelease))(i);
				(this->*(buttons[i].onPressed))(i, millis() - buttons[i].lastUpdate);
				buttons[i].lastUpdate = millis();
			}
		}
	}
}

/* splash screen handling main loop */
ScreenLoop AnalyzerApp::splashScreenLoop() {
	switch (applicationState) {
		case wait:
			if (millis() - counters[mainCounter] > SPLASH_SCREEN_DURATION_MS) {
				Serial.println(F("SplashScreen time exceeded. Lauching manual activity."));
				applicationState = displayMainScreen;
				return ManualScreen; // Quit current screen to the manual screen
			}
			break;
		case displaySplashScreen:
			ozOledDisplayAdapter.displaySplashScreen();		   // the splash screen
			counters[mainCounter] = millis();
			applicationState = wait;
			break;
		default:
			Serial.print(F("Default in splashScreenLoop command : "));
			Serial.println(applicationState);
	}

	return SplashScreen;
}

/* do a manual calibration */
ApplicationStates AnalyzerApp::doCalibration(ApplicationStates current, ApplicationStates next) {
	long value;
	ApplicationStates returnState = current;

	switch (subFunctionState) {
	default:
		Serial.println(F("Starting calibration"));
		averager.init();
		subFunctionState = 1;
		// Displays progress bar and message
		ozOledDisplayAdapter.drawText(8, 4, "Calibrating...");
		memset(displayDigits, 11, 8);

	case 1: //iterate
		if (newADCValueAvailable) {
			newADCValueAvailable = false;
			averager.addValue(ADS1210.getDigitalOutputValue());

			ozOledDisplayAdapter.drawProgressBar(0, 7, map(averager.getCounter(), 0, CALIBRATION_MAX_ITERATIONS, 0, 100));

			if (averager.getCounter() == CALIBRATION_MAX_ITERATIONS ||
				(btConnection && averager.getCounter() == ((GetAverageCommand *)command)->getIterations())) {
				subFunctionState = 2;
			}
		}
		break;
	case 2:
		calibrationValue = averager.getAverage();

		if (btConnection) {
			((GetAverageCommand *)command)->setAverage(calibrationValue);
			inOutIconUpdate(true, false);
			protocolHandler.sendResponse(command);
		}

		convertDecimalToDigits(2095);

		long avrMin = averager.getMin();
		long avrMax = averager.getMax();
		long avrAv = averager.getAverage();
		sprintf(buffer, "%li [%li, %li]\0", avrAv, avrMin - avrAv, avrMax - avrAv);
		Serial.println(buffer);

		//clear screen and display
		ozOledDisplayAdapter.clearDisplay(2, 7, 0, 127);
		ozOledDisplayAdapter.drawOxygenRate(8, 3, displayDigits);

		subFunctionState = 0;
		returnState = next;
		break;
	}

	return returnState;
}

/* manual handling main loop */
ScreenLoop AnalyzerApp::manualScreenLoop() {
	switch (applicationState) {
		case displayMainScreen:
			Serial.println(F("Drawing main interface"));
			drawsManualMainScreen();
			applicationState = idle;
			break;
		case calibrate :
			applicationState = doCalibration(calibrate, idle);
			break;
		case measure:
			applicationState = doO2Measure(measure, idle);
			break;
		case idle:
			//updates bt icon
			if (counters[bluetoothIcon] && (millis() - counters[bluetoothIcon]) > 250) {
				counters[bluetoothIcon] = millis();
				bluetoothState = ++bluetoothState % 4;
				ozOledDisplayAdapter.drawBluetoothIconState(64, 0, bluetoothState);
			}

			if (autoCalibrate && calibrationValue == 0) { applicationState = calibrate;  Serial.println(F("Selecting Calibrate action")); }
			if (autoMeasure && calibrationValue != 0) { applicationState = measure; Serial.println(F("Selecting Measure action")); }

			break;
		default:
			Serial.print(F("Default in manualScreenLoop command : "));
			Serial.println(applicationState);
	}

	return ManualScreen;
}

/* Bluetooth handling main loop */
ScreenLoop AnalyzerApp::bluetoothScreenLoop(){

	switch (applicationState) {
		case getO2CellInstallDate:
			Serial.println(F("Returning the o2 cell install date"));
			((GetO2CellInstallDateCommand *)command)->setDate(eeprom.configuration.O2Cell.install_date.day, eeprom.configuration.O2Cell.install_date.month, eeprom.configuration.O2Cell.install_date.year);
			protocolHandler.sendResponse(command);
			applicationState = idle;
			break;
		case getO2CellValidityDate:
			Serial.println(F("Returning the o2 cell validity date"));
			((GetO2CellInstallDateCommand *)command)->setDate(eeprom.configuration.O2Cell.change_date.day, eeprom.configuration.O2Cell.change_date.month, eeprom.configuration.O2Cell.change_date.year);
			protocolHandler.sendResponse(command);
			applicationState = idle;
			break;
		case idle:
			applicationState = getNextCommand(idle);
		
			if (counters[inOutCounter] > 0 && millis() - counters[inOutCounter] > 500) {
				//If no more activity clear icon
				inOutIconUpdate(false, false);
				counters[inOutCounter] = 0;
			}
			
			if (!protocolHandler.isBTConnectionUp()) {
				Serial.println(F("Bluetooth Timeout detected"));
				btConnection = false;
				applicationState = displayMainScreen; //Set application loop state to idle
				return ManualScreen; //Change application main loop to the one handling bluetooth coms
			}
			break;
		default:
			Serial.print(F("Default in bluetoothScreenLoop command : "));
			Serial.println(applicationState);
	}

	return BluetoothScreen; //default main loop screen
}

/* called each time a new adc value is produced */
void adcIRQ() {
	app.setNewADCValueAvailable(true);
	ADS1210.readRegisters();
}

/* Draws the main window while in manual mode */
void AnalyzerApp::drawsManualMainScreen() {
	ozOledDisplayAdapter.clearDisplay(); //clears display

	if (!autoCalibrate)
		ozOledDisplayAdapter.drawBitmap(0, 0, calibMenu);

	if (!autoMeasure) //Draws only when not in auto mode
		ozOledDisplayAdapter.drawBitmap(0, 0, startMenu);

	ozOledDisplayAdapter.drawBitmap(112, 0, settingsIcon);

	ozOledDisplayAdapter.drawBatteryIcon(47, 0, getBatteryMappedValue()); //draw battery status
	ozOledDisplayAdapter.drawBluetoothIconState(64, 0, bluetoothState); // draw bluetooth status
	ozOledDisplayAdapter.drawDataInOutIcon(86, 0, dataInOutState); // draw data in out status

	counters[bluetoothIcon] = millis();

	Serial.println(F("Drawed main window in manual mode"));
}

/* Returns a value between 0 and 100 that maps battery voltage between 2.2V and 3.7V */
int AnalyzerApp::getBatteryPercent() {
	int ar = analogRead(BATTERY_ANALOG_PIN);
	int mapped = map(ar, 450, 757, 0, 100);

	if (ar < 450)
		mapped = 0;
	if (ar > 757)
		mapped = 100;
}

/* Maps 0-100% to 0-4 for display*/
int AnalyzerApp::getBatteryMappedValue() {
	return map(getBatteryPercent(), 0, 100, 0, 4);
}

/* Check that protocol handler received a new command and execute it */
ApplicationStates AnalyzerApp::getNextCommand(ApplicationStates current) {
	command = protocolHandler.getNextCommand();

	if (command != NULL) {
		inOutIconUpdate(false, true);

		Serial.print(F("Command ID : "));
		Serial.println(command->getCommandID());

		switch (command->getCommandID()) {
		case GET_O2_CELL_INSTALL_DATE:
			return getO2CellInstallDate;
		case GET_O2_CELL_VALIDITY_DATE:
			return getO2CellValidityDate;
		case GET_AVERAGE:
			
			//if (((GetAverageCommand *)command)->getIterations() > 30) {
			//Considered as calibration
			//return calibrate;
			//}
			//else {
			//return measure;
			//}
		case GET_BATTERY_LOAD:
//			return getBatteryLoad;
		case SET_O2_CELL_INSTALL_DATE:
//			return setO2CellInstallDate;
		default:
			return current;
		}
	}
	else{
		return current;
	}

	return current;
}

/* Updates the data inOut icon */
void AnalyzerApp::inOutIconUpdate(bool send, bool receive) {
	dataInOutState = receive ? (send ? inout : in) : (send ? out : none);
	ozOledDisplayAdapter.drawDataInOutIcon(85, 0, dataInOutState);
	counters[inOutCounter] = millis();
}

/* Finds the right digit #Tested */
long AnalyzerApp::fromIntToDigitsArray(int digit, long decvalcp, long power) {
	byte tmp = (byte)((decvalcp - (decvalcp % power)) / power);
	displayDigits[digit] = tmp;
	decvalcp = decvalcp - tmp * power;
	return decvalcp;
}

/*Displays the Oxy percent value #Tested */
void AnalyzerApp::convertDecimalToDigits(long decval) {
	long decvalcp = decval;

	decvalcp = fromIntToDigitsArray(0, decvalcp, 10000);
	decvalcp = fromIntToDigitsArray(1, decvalcp, 1000);
	decvalcp = fromIntToDigitsArray(2, decvalcp, 100);

	displayDigits[3] = 10; // Point

	decvalcp = fromIntToDigitsArray(4, decvalcp, 10);
	decvalcp = fromIntToDigitsArray(5, decvalcp, 1);

	displayDigits[6] = 12;
	displayDigits[7] = 11;
	displayDigits[8] = 13;

	if (((int)decval / 100.0) < 10){
		displayDigits[0] = 11;
		displayDigits[1] = 11;
	}

	if (((int)decval / 100.0) < 100){
		displayDigits[0] = 11;
	}
}

/* Do an o2 measure */
ApplicationStates AnalyzerApp::doO2Measure(ApplicationStates current, ApplicationStates next) {
	long value;

	switch (subFunctionState){
	default:
		averager.init();
		memset(displayDigits, 11, 8);
		subFunctionState = 1;
	case 1: //iterate
		if (newADCValueAvailable) {
			newADCValueAvailable = false;	
			averager.addValue(ADS1210.getDigitalOutputValue());			
			ozOledDisplayAdapter.drawProgressBar(0, 7, map(averager.getCounter(), 0, O2_READ_ITERATIONS, 0, 100));

			if (averager.getCounter() == O2_READ_ITERATIONS ||
				(btConnection && averager.getCounter() == ((GetAverageCommand *)command)->getIterations())) {
				subFunctionState = 2;
			}
		}
		break;
	case 2:
		//oldAverage = average;
		average = (20.95 / (double)calibrationValue) * averager.getAverage();
		convertDecimalToDigits((long)(average * 100));

		long avrMin = averager.getMin();
		long avrMax = averager.getMax();
		long avrAv = averager.getAverage();
		
		sprintf(buffer, "%li [%li, %li]\0", avrAv, avrMin - avrAv, avrMax - avrAv);
		Serial.println(buffer);

		//clear screen and display
		ozOledDisplayAdapter.clearDisplay(2, 7, 0, 127);
		ozOledDisplayAdapter.drawOxygenRate(8, 3, displayDigits);

		/*
		if (!((oldAverage - 0.05) < average && average < (oldAverage + 0.05))) {
			oldAverage = average;

			ozOledDisplayAdapter.clearDisplay(2, 7, 0, 127);
			ozOledDisplayAdapter.drawOxygenRate(8, 3, displayDigits);
		}
		else{
			ozOledDisplayAdapter.clearDisplay(7, 7, 0, 127);
		}
		*/
		subFunctionState = 0;
		return next;
	}

	return current;
}















































































/*





bool AnalyzerApp::batteryStateChanged() {
	int val = getBatteryMappedValue();
	
	if (val != batteryState) {
		batteryState = val;		
		return true;
	}

	return false;
}

void AnalyzerApp::screenDisplayLoop() {
	switch (screenDisplayState)
	{
		
		case displayBTConnectedScreen:
			ozOledDisplayAdapter.clearDisplay();

			if (!autoMeasure)
				ozOledDisplayAdapter.drawBitmap(8, 0, startMenu);
			if (!autoCalibrate)
				ozOledDisplayAdapter.drawBitmap(88, 0, calibMenu);

			ozOledDisplayAdapter.drawBatteryIcon(47, 0, getBatteryMappedValue());
			bluetoothState = 3;
			ozOledDisplayAdapter.drawBluetoothIconState(64, 0, bluetoothState);

			ozOledDisplayAdapter.drawText(8, 4, "Bluetooth link");
			ozOledDisplayAdapter.drawText(24, 5, "activated!");

			screenDisplayState = idle;
			break; 
		case idle:
			if (!btConnection && counters[bluetoothIcon] > 0 && millis() - counters[bluetoothIcon] > 250) {
				counters[bluetoothIcon] = millis();
				bluetoothState = ++bluetoothState % 4;
			}

			if (counters[inOutCounter] > 0 && millis() - counters[inOutCounter] > 500) {
				//If no more activity clear icon
				inOutIconUpdate(false, false);
				counters[inOutCounter] = 0;
			}

			if (batteryStateChanged()) {ozOledDisplayAdapter.drawBatteryIcon(47, 0, batteryState);}
			if (dataInOutChanged()) {ozOledDisplayAdapter.drawDataInOutIcon(85, 0, dataInOutState);}
			if (bluetoothStateChanged()) {ozOledDisplayAdapter.drawBluetoothIconState(64, 0, bluetoothState);}

			break;
		default:
			//Do nothing
			break;
	}
}

bool AnalyzerApp::dataInOutChanged() {
	bool ret = dataInOutState != oldDataInOutState;
	oldDataInOutState = dataInOutState;
	return ret;
}

bool AnalyzerApp::bluetoothStateChanged() {
	bool ret = bluetoothState != oldBluetoothState;
	oldBluetoothState = bluetoothState;
	return ret;
}

void AnalyzerApp::bluetoothDoLoop() {
	unsigned int mappedVal = 0, val = 0;
	long value;

	//Handle protocol
	switch (bluetoothApplicationStates) {
	case STATE_EXECUTE_GET_AVERAGE_INIT:
		doCalibration(STATE_EXECUTE_GET_AVERAGE_READ_VALUES, STATE_IDLE);
		//
		//averager.init();
		//maxIterations = ((GetAverageCommand *)command)->getIterations();
		//bluetoothApplicationStates = STATE_EXECUTE_GET_AVERAGE_READ_VALUES;
		//
		break;
	case STATE_EXECUTE_GET_AVERAGE_READ_VALUES:
		if (averager.getCounter() == maxIterations) { 
			bluetoothApplicationStates = STATE_EXECUTE_GET_AVERAGE_RETURN_RESPONSE; }
		else {
			if (newADCValueAvailable) {
				newADCValueAvailable = false;
				averager.addValue(ads1210.getDigitalOutputValue());
			}
		}
		break;
	case STATE_EXECUTE_GET_AVERAGE_RETURN_RESPONSE:
		average = averager.getAverage();
		((GetAverageCommand *)command)->setAverage(average);
		protocolHandler.sendResponse(command);

		sprintf(buffer, "Average ater %i iterations : (min, max, max-min, average)=>(%li, %li, %li, %li)\0", maxIterations, min, max, max - min, average);
		Serial.println(buffer);

		counter = 0;
		average = 0;
		min = 0x7FFFFFFF;
		max = 0x80000000;
		bluetoothApplicationStates = STATE_IDLE;
		break;
	case STATE_EXECUTE_GET_BATTERY_LOAD:
		//long v = analogRead(BATTERY_AD_INPUT);
		//When battery is full  A0 reads value 4.4 Volts => 0.55 * 8V
		//		int value = map(analogRead(BATTERY_AD_INPUT), 0, 1023, 0, 5);
		//9 = 4.5 = batter full
		//6 = 3 = battery quite empty

		// ==> 950 max  600 min
		val = analogRead(BATTERY_ANALOG_PIN);
		delayMicroseconds(100);
		val = analogRead(BATTERY_ANALOG_PIN);
		Serial.print("Analog read A0 = ");
		Serial.println(val);

		if (val < 650)
			mappedVal = 0;
		if (val > 850)
			mappedVal = 100;
		if (val >= 650 && val <= 850)
			mappedVal = (int)((val - 650) / 2.0);

		Serial.print("Mapped read A0 = ");
		Serial.println(mappedVal);
		((GetBatteryLoadCommand *)command)->setLoad(mappedVal);
		protocolHandler.sendResponse(command);
		bluetoothApplicationStates = STATE_IDLE;
		break;
	case STATE_EXECUTE_GET_O2_CELL_INSTALL_DATE:
		if (isEEPROMClean()) {
			((GetO2CellInstallDateCommand *)command)->setDate(eeprom.O2Cell.install_date.day, eeprom.O2Cell.install_date.month, eeprom.O2Cell.install_date.year);
			protocolHandler.sendResponse(command);
		}
		else{

			protocolHandler.sendErrorResponse(1, "No valid clean install date found");
		}
		bluetoothApplicationStates = STATE_IDLE;
		break;
	case STATE_EXECUTE_SET_O2_CELL_INSTALL_DATE:
		eeprom.O2Cell.install_date.day = ((SetO2CellInstallDateCommand *)command)->getDay();
		eeprom.O2Cell.install_date.month = ((SetO2CellInstallDateCommand *)command)->getMonth();
		eeprom.O2Cell.install_date.year = ((SetO2CellInstallDateCommand *)command)->getYear();

		if (eeprom.clean != 0xCAFEBABE) {
			eeprom.clean = 0xCAFEBABE;
			eeprom.O2Cell.change_date.day = eeprom.O2Cell.install_date.day;
			eeprom.O2Cell.change_date.month = eeprom.O2Cell.install_date.month;
			eeprom.O2Cell.change_date.year = eeprom.O2Cell.install_date.year + 3;
		}

		//Load eeprom
		for (int i = 0; i < EEPROM_STORED_DATA_SIZE; i++) {
			EEPROM.write(EEPROM_CONFIG_START_ADDRESS + i, eeprom.data[i]);
		}

		protocolHandler.sendResponse(command);
		bluetoothApplicationStates = STATE_IDLE;
		break;
	case DISPLAY_SCREEN_ICONS:
		bluetoothState = 3;
		ozOledDisplayAdapter.clearDisplay(3, 7, 0, 127);
		ozOledDisplayAdapter.drawBluetoothIconState(64, 0, bluetoothState);
		ozOledDisplayAdapter.drawText(3, 4, "connected!");
		bluetoothApplicationStates = idle;
	case idle:
	default:
		command = protocolHandler.getNextCommand();

		if (command != NULL) {
			Serial.print(F("Command ID : "));
			Serial.println(command->getCommandID());
			Serial.print(F("command received : "));
			Serial.println((command)->getName());

			switch ((int)command->getCommandID()) {
			case GET_AVERAGE:
				subFunctionState = 0;
				bluetoothApplicationStates = STATE_EXECUTE_GET_AVERAGE_INIT;
				break;
			case GET_BATTERY_LOAD:
				bluetoothApplicationStates = STATE_EXECUTE_GET_BATTERY_LOAD;
				break;
			case GET_O2_CELL_INSTALL_DATE:
				bluetoothApplicationStates = STATE_EXECUTE_GET_O2_CELL_INSTALL_DATE;
				break;

			case SET_O2_CELL_INSTALL_DATE:
				bluetoothApplicationStates = STATE_EXECUTE_SET_O2_CELL_INSTALL_DATE;
				break;
			default:
				bluetoothApplicationStates = idle;
				break;
			}
		}
	}
}








*/


AnalyzerApp app;
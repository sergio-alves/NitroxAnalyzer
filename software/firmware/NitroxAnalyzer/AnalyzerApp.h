#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Wire.h>
#include <OzOLED.h>
#include "ADS1210Driver.h"
#include "OzOledDisplayAdapter.h"
#include "SerialProtocolHandler.h"
#include "Averager.h"


/*
enum ApplicationStates {

autoCalibrateAction,
autoMesuringAction,
refreshBatteryState,
refreshDataInOutState,
refreshBluetoothState,
displayBTConnectedScreen,
getBatteryLoad,
setO2CellInstallDate,
setO2CellValidityDate,
STATE_IDLE,
STATE_EXECUTE_GET_AVERAGE_INIT,
STATE_EXECUTE_GET_AVERAGE_READ_VALUES,
STATE_EXECUTE_GET_AVERAGE_RETURN_RESPONSE,
STATE_EXECUTE_GET_BATTERY_LOAD,
STATE_EXECUTE_GET_O2_CELL_INSTALL_DATE,
STATE_EXECUTE_SET_O2_CELL_INSTALL_DATE,
DISPLAY_SCREEN_ICONS,
};








*/

//----------------------------------------------------------------------------------------------------------
// Externals -----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
extern void adcIRQ();
class AnalyzerApp;

//----------------------------------------------------------------------------------------------------------
// Definitions ---------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

#define isEEPROMClean() eeprom.configuration.clean==0xCAFEBABE

// pins related 

#define BATTERY_ANALOG_PIN				A0

#define ADS1210_CLK						8
#define ARDUINO_OUT_ADS1210_IN			10
#define ARDUINO_IN_ADS1210_OUT			4


//buttons related
#define LEFT_BUTTON						11
#define RIGHT_BUTTON					12

#define DEBOUNCE_MAX_MS					20

typedef struct {
	unsigned long lastUpdate;
	int currentState;
	void(AnalyzerApp::*onPush)(int);
	void(AnalyzerApp::*onRelease)(int);
	void(AnalyzerApp::*onPressed)(int, int);
} ButtonState;


// counters related
#define COUNTERS_SIZE					4

enum  Counters {
	mainCounter,
	bluetoothIcon,
	inOutCounter,
	bluetoothConnectionAlive,
};


//eeprom related 
#define EEPROM_STORED_DATA_SIZE			12
#define EEPROM_CONFIG_START_ADDRESS		0

typedef struct {
	int day;
	int month;
	int year;
} date;

typedef struct {
	long clean;
	struct {
		date install_date; //6 bytes
		date change_date;  //6 bytes
	} O2Cell; // 12 bytes
} AnalyzerConfiguration;

typedef union {
	byte data[sizeof(AnalyzerConfiguration)];
	AnalyzerConfiguration configuration;
} EEPROMContent;

//communication speed related
#define SOFTWARE_RX_PIN					5
#define SOFTWARE_TX_PIN					6

#define SERIAL_BAUDRATE					115200
#define BLUETOOTH_BAUDRATE				9600


// screens and loops related
#define SPLASH_SCREEN_DURATION_MS		2000

enum ScreenLoop {
	SplashScreen,
	ManualScreen,
	BluetoothScreen,
};
enum ApplicationStates {
	idle,
	wait,
	calibrate,
	measure,
	displaySplashScreen,
	displayMainScreen,

	getO2CellInstallDate,
	getO2CellValidityDate,
};

enum DataInOutState{
	none,
	in,
	out,
	inout,
};

/* Calibration related */
#define CALIBRATION_MAX_ITERATIONS  100
#define O2_READ_ITERATIONS			10


/* Class to handle the analyzer application.
   Calibration and measures can be done using the box or through the bt interface (if available)
   then the structure must be rather flexible.
   On BT Connection device will answer to bt commands only, no device interaction possible. */
class AnalyzerApp
{
public:
	void doLoop();
	void doSetup();
	inline void setNewADCValueAvailable(bool val) { newADCValueAvailable = val; }

protected:
	/* button events related functions */
	void onButtonPressed(int btnId, int length);
	void onButtonRelease(int btnId);
	void onButtonPush(int btnId);
	void poolButtonsState();
	bool stateChangedDebounced(int);

	/* sub loops related */
	ScreenLoop splashScreenLoop();
	ScreenLoop manualScreenLoop();
	ScreenLoop bluetoothScreenLoop();

	/* displaying related*/
	void drawsManualMainScreen();
	int getBatteryPercent();
	int getBatteryMappedValue();
	void inOutIconUpdate(bool send, bool receive);
	void convertDecimalToDigits(long decval);
	long fromIntToDigitsArray(int digit, long decvalcp, long power);

	/* bluetooth serial communication related */
	ApplicationStates getNextCommand(ApplicationStates current);

	/* eeprom related */
	void loadConfigurationFromEEPROM();

	/* manual octions related*/
	ApplicationStates doCalibration(ApplicationStates current, ApplicationStates next);
	ApplicationStates doO2Measure(ApplicationStates current, ApplicationStates next);


private:
	SoftwareSerial BTSerial = SoftwareSerial(SOFTWARE_RX_PIN, SOFTWARE_TX_PIN);
	SerialProtocolHandler protocolHandler = SerialProtocolHandler(BTSerial);
	EEPROMContent eeprom;
	ApplicationStates applicationState;
	ButtonState buttons[2];
	bool btConnection = false;
	ScreenLoop currentLoop = SplashScreen;
	unsigned long counters[COUNTERS_SIZE];
	bool newADCValueAvailable = false;
	bool autoCalibrate = true;
	bool autoMeasure = true;
	DataInOutState dataInOutState = none;
	int bluetoothState = 0;
	SerialCommand * command;
	int subFunctionState = 0;
	Averager averager;
	double average,  oldAverage;
	byte displayDigits[9];
	long calibrationValue = 0;
	char buffer[128];



	/*
protected:
	void bluetoothDoLoop();
	void screenDisplayLoop();
	bool dataInOutChanged();
	bool bluetoothStateChanged();
	bool batteryStateChanged();
private:	
	DataInOutState oldDataInOutState = none;
	int oldBluetoothState = 0;
	int batteryState = 0;
	int oldBatteryState = 0;
	ApplicationStates screenDisplayState = idle;
	ApplicationStates bluetoothApplicationStates = idle;
	int counter = 0, iterations=0;
	unsigned long average = 0;
	long min = 0x7FFFFFFF, max = 0x80000000;
	bool numDigitFound = false;
	SendingBuffer sb;
	DWORD_MAX_187_DATA_FORMAT current;
	
	char c;
	
	int maxIterations = 10;*/
};

extern AnalyzerApp app;

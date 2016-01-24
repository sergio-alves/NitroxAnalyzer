
#include <SoftwareSerial.h>
#include <SPI.h>
#include <stdio.h>
#include <EEPROM\EEPROM.h>
#include "DS1868Driver.h"

#include "ADS1210Driver.h"
#include "SerialProtocolHandler.h"

#define SOFTWARE_RX_PIN					5
#define SOFTWARE_TX_PIN					6

#define RESET_PIN						9
#define CLOCK_PIN						8
#define DATA_PIN						7

#define ADS1210_CLK						8
#define ARDUINO_OUT_ADS1210_IN			10
#define ARDUINO_IN_ADS1210_OUT			4

#define MAX1987_CS						10
#define BATTERY_AD_INPUT				A0

SoftwareSerial BTSerial = SoftwareSerial(SOFTWARE_RX_PIN, SOFTWARE_TX_PIN);
SerialProtocolHandler protocolHandler = SerialProtocolHandler(BTSerial);

#define EEPROM_STORED_DATA_SIZE			12
#define EEPROM_CONFIG_START_ADDRESS		0

typedef struct {
	int day;
	int month;
	int year;
} date;

typedef union {
	byte data[EEPROM_STORED_DATA_SIZE];
	struct {
		long clean;
		struct {
			date install_date; //6 bytes
			date change_date;  //6 bytes
		} O2Cell; // 12 bytes
	};
} EEPROMContent;

EEPROMContent eeprom;

#define isEEPROMClean() eeprom.clean==0xCAFEBABE

typedef union {
	struct {
		unsigned crap : 1;
		unsigned value : 12;
		unsigned padding : 3;
	} bitfield;
	struct {
		byte low;
		byte high;
	}bytes;
	unsigned int raw;
} DWORD_MAX_187_DATA_FORMAT; //2 bytes

typedef union {
	char bytes[6];
	struct {
		unsigned int start;
		unsigned int current;
		unsigned int end;
	}structuredData;
} SendingBuffer;

SendingBuffer sb;
DWORD_MAX_187_DATA_FORMAT current;
char buffer[128];
char c;
//char incomingBuffer[64];

/* This app setup*/
void setup()
{
	Serial.begin(115200);

	BTSerial.begin(9600);

	SPI.begin();

	pinMode(MAX1987_CS, OUTPUT);
	digitalWrite(MAX1987_CS, HIGH);

	/* Begin DS1868 driver module */
	ds1868.begin(RESET_PIN, CLOCK_PIN, DATA_PIN);

	attachInterrupt(digitalPinToInterrupt(2), newMeasure, FALLING);

	ads1210.begin(ADS1210_CLK, ARDUINO_OUT_ADS1210_IN, ARDUINO_IN_ADS1210_OUT);

	pinMode(BATTERY_AD_INPUT, INPUT);

	//Load eeprom
	for (int i = 0; i < EEPROM_STORED_DATA_SIZE; i++) {
		eeprom.data[i] = EEPROM.read(EEPROM_CONFIG_START_ADDRESS + i);
	}
}

bool readMeasure = false;


void newMeasure() {
	readMeasure = true;
}

/* get multiple times the digital value and average it */
int averagedDigitalValue(int reads) {
	double average = 0;

	SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE0));

	for (int i = 0; i < reads; i++) {
		//start conversion
		digitalWrite(MAX1987_CS, LOW);

		//wait for conversion to finish 1.5 for track+hold + 8.5 for conversion 20 >>> 10.5
		delayMicroseconds(20);

		//get converted data
		current.bytes.high = SPI.transfer(0);
		current.bytes.low = SPI.transfer(0);

		//get the cleaned value
		average += current.bitfield.value;

		//Stop conversion
		digitalWrite(MAX1987_CS, HIGH);
	}

	SPI.endTransaction();

	return (int)(average / reads);
}

unsigned long average;
int counter = 0;
int maxIterations = 10;
long min = 0x7FFFFFFF;
long max = 0x80000000;

void readAndIncrement() {
	long l;

	if (readMeasure) {
		l = ads1210.readDataOutputRegister();

		if (l < min)
			min = l;
		if (l > max)
			max = l;

		average += l;
		counter++;
	}
}

enum States {
	STATE_IDLE,
	STATE_EXECUTE_GET_AVERAGE_INIT,
	STATE_EXECUTE_GET_AVERAGE_READ_VALUES,
	STATE_EXECUTE_GET_AVERAGE_RETURN_RESPONSE,
	STATE_EXECUTE_GET_BATTERY_LOAD,
	STATE_EXECUTE_GET_O2_CELL_INSTALL_DATE,
	STATE_EXECUTE_SET_O2_CELL_INSTALL_DATE,
};

SerialCommand * command;
States state = STATE_IDLE;


void loop() {
	unsigned int mappedVal = 0, val = 0;

	//Handle protocol
	switch (state) {
	case STATE_EXECUTE_GET_AVERAGE_INIT:
		maxIterations = ((GetAverageCommand *)command)->getIterations();
		state = STATE_EXECUTE_GET_AVERAGE_READ_VALUES;
		break;
	case STATE_EXECUTE_GET_AVERAGE_READ_VALUES:
		if (counter == maxIterations) { state = STATE_EXECUTE_GET_AVERAGE_RETURN_RESPONSE; }
		else { readAndIncrement(); }
		break;
	case STATE_EXECUTE_GET_AVERAGE_RETURN_RESPONSE:
		average = (long)(average / (float)maxIterations);
		((GetAverageCommand *)command)->setAverage(average);

		protocolHandler.sendResponse(command);

		sprintf(buffer, "Average ater %i iterations : (min, max, max-min, average)=>(%li, %li, %li, %li)\0", maxIterations, min, max, max - min, average);
		Serial.println(buffer);

		counter = 0;
		average = 0;
		min = 0x7FFFFFFF;
		max = 0x80000000;
		state = STATE_IDLE;
		break;
	case STATE_EXECUTE_GET_BATTERY_LOAD:
		//long v = analogRead(BATTERY_AD_INPUT);
		//When battery is full  A0 reads value 4.4 Volts => 0.55 * 8V
		//		int value = map(analogRead(BATTERY_AD_INPUT), 0, 1023, 0, 5);
		//9 = 4.5 = batter full
		//6 = 3 = battery quite empty

		// ==> 950 max  600 min
		val = analogRead(BATTERY_AD_INPUT);
		delayMicroseconds(100);
		val = analogRead(BATTERY_AD_INPUT);
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
		state = STATE_IDLE;
		break;
	case STATE_EXECUTE_GET_O2_CELL_INSTALL_DATE:
		if (isEEPROMClean()) {
			((GetO2CellInstallDateCommand *)command)->setDate(eeprom.O2Cell.install_date.day, eeprom.O2Cell.install_date.month, eeprom.O2Cell.install_date.year);
			protocolHandler.sendResponse(command);
		}
		else{
			
			protocolHandler.sendErrorResponse(1, "No valid clean install date found");
		}
		state = STATE_IDLE;
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
		state = STATE_IDLE;
		break;
	case STATE_IDLE:
	default:
		command = protocolHandler.getNextCommand();

		if (command != NULL) {
			switch ((int)command->getCommandID()) {
			case GET_AVERAGE:
				state = STATE_EXECUTE_GET_AVERAGE_INIT;
				break;
			case GET_BATTERY_LOAD:
				state = STATE_EXECUTE_GET_BATTERY_LOAD;
				break;
			case GET_O2_CELL_INSTALL_DATE:
				state = STATE_EXECUTE_GET_O2_CELL_INSTALL_DATE;
				break;

			case SET_O2_CELL_INSTALL_DATE:
				state = STATE_EXECUTE_SET_O2_CELL_INSTALL_DATE;
				break;
			default:
				state = STATE_IDLE;
				break;
			}
		}
	}

	//readMeasure is updated by interrupt each type a new value from ads1210 is available
	readMeasure = false;
}

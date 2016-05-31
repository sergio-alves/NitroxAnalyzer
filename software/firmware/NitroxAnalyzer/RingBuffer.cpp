// 
// 
// 

#include "RingBuffer.h"

void RingBufferClass::init(int size)
{
	values = new long[size];
	RingBufferClass::size = size;
	pointer = 0;
	memset(values, 0, size);
}

void RingBufferClass::attachCountDownEvent(int c, void(*f)(long, long, long)) {
	funtiontocall = f;
	partSize = c;
	target = (pointer+c) % size;
	Serial.print(F("target : "));
	Serial.println(target);
};

void RingBufferClass::addValue(long value) {
	Serial.print(F("Adding value : "));
	Serial.print(value);
	Serial.print(F(" to pointer "));
	Serial.println(pointer);

	values[pointer%size] = value;
	if (min > value)
		min = value;
	if (max < value)
		max = value;

	pointer++;
	pointer %= size;
	addedSinceEventAttached++;

	if (funtiontocall != NULL) {
		if (partMin > value)
			partMin = value;
		if (partMax < value)
			partMax = value;

		partAverage += value;

		Serial.print(F("Function to call set and actual min, max => "));
		Serial.print(partMin);
		Serial.print(F(", "));
		Serial.println(partMax);

		if (pointer == target) {
			Serial.print(F("pointer reached min, max, average => "));
			Serial.print(partMin);
			Serial.print(F(", "));
			Serial.print(partMax);
			Serial.print(F(", "));
			Serial.println((long)(partAverage / (double)partSize));
			

			funtiontocall(partMin, partMax, (long)(partAverage / (double)partSize));
			partMin = 0x7fffffff;
			partMax = 0x80000000;
			partAverage = 0;
			addedSinceEventAttached = 0;
		}
	}


}

long RingBufferClass::getMax() {
	return max;
}

long RingBufferClass::getMin() {
	return min;
}

long RingBufferClass::getAverage(int c) {
	long av = 0;
	int cnt = c;

	for (int i = 0; i < c; i++) {
		if ((pointer - i) >= 0)
			av = values[pointer - i];
		else
			av = values[size + ((pointer - i) % size)];
	}

	return (long)(av / (float)c);
}

long RingBufferClass::getAverage() {
	for (int i = 0; i < size; i++) {
		average += values[i];
	}

	return (long)(average / (float)size);
}


RingBufferClass RingBuffer;


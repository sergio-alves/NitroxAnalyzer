#include "Averager.h"

void Averager::init() {
	_summer = 0;
	_counter = 0;
	__min = 0x7FFFFFFF;
	__max = 0x80000000;
}

long Averager::addValue(long value) {
	_summer += value;
	_counter++;
	if (value < __min){
		__min = value;
	}

	if (value > __max) {
		__max = value;
	}

	return (long)(_summer / (double)_counter);
}

long Averager::getMax() {
	return __max;
}
	
long Averager::getMin(){
	return __min;
}

long Averager::getAverage() {
	return (long)(_summer / (double)_counter);
}

long Averager::getCounter() {
	return _counter;
}

#pragma once

#include <Arduino.h>

class Averager
{
	protected:
		unsigned long _summer, _counter;
		long __min, __max;

	public:
		void init();
		long addValue(long value);
		long getMax();
		long getMin();
		long getAverage();
		long getCounter();
};


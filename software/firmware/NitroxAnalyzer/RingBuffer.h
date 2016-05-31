// RingBuffer.h

#ifndef _RINGBUFFER_h
#define _RINGBUFFER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class RingBufferClass
{
 protected:
	 long * values;
	 int pointer = 0;
	 int size = 0;
	 long min = 0x7fffffff, max = 0x80000000, average = 0;
	 long partMin = 0x7fffffff, partMax = 0x80000000, partAverage = 0;
	 void (*funtiontocall)(long, long, long) = NULL;
	 int target=-1;
	 int partSize;
	 int addedSinceEventAttached = 0;
	 

 public:
	void init(int size);
	void addValue(long value);
	long getMax();
	long getMin();
	long getAverage();
	long getAverage(int c);
	void attachCountDownEvent(int counter, void(*f)(long, long, long)) ;
	inline int getAddedSinceEventAttached() { return addedSinceEventAttached; }
};

extern RingBufferClass RingBuffer;

#endif


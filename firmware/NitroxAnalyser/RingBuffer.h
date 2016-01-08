#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

//template <class T>
class RingBuffer
{
private:
	int * buffer;
	int in;
	int out;
	int count = 0;
	int size = 0;

public:
  RingBuffer();
	RingBuffer(int size);
	~RingBuffer();

	int insert(int data);
	int remove();
	int getCount();
	bool isFull();
	bool isEmpty();
	int getElementAt(int index);
};

#endif //_RING_BUFFER_H

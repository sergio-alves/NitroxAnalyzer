#include "RingBuffer.h"

//template <class T> 
RingBuffer::RingBuffer() {
  size = 10;
  buffer = new int[size];
  in = 0;
  out = 0;
  count = 0;
}

//template <class T> 
RingBuffer::RingBuffer(int s)
{
	size = s;
	buffer = new int[s];
	in = 0;
	out = 0;
	count = 0;
}

RingBuffer::~RingBuffer()
{
	free(buffer);
}

int RingBuffer::remove(){
	byte data = buffer[out];
	buffer[out] = 0;

	if (++out == size)
		out = 0;

	count--;

	return data;
}

int RingBuffer::insert(int data) {
	buffer[in] = data;

	if (++in == size) {
		in = 0;
	}
	
	if (in == out){
		remove();
	}

	count++;

	return data;
}

int RingBuffer::getCount() {
	return count;
}


bool RingBuffer::isFull()
{
	return count == size;
}

bool RingBuffer::isEmpty()
{
	return count == 0;
}

int RingBuffer::getElementAt(int index) {
	if (index > size)
		return NULL;

	return  buffer[(out + index) % size];
}

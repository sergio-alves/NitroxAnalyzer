#pragma once




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

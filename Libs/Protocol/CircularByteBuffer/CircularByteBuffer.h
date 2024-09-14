#pragma once

#include "PacketBase/PacketBase.h"
#include <stdint.h>

class CircularByteBuffer
{
public:
	explicit CircularByteBuffer(const uint32_t size);
	~CircularByteBuffer() {if (_pBuffer) delete [] _pBuffer;}

	uint8_t* pBuffer() const {return _pBuffer;}
	uint32_t size() const {return _size;}
	void clear();
	bool receive(const uint8_t* data, const uint32_t len);
	bool getNewData(PacketBase* packet);
	void moveTail(const uint32_t len);

private:
	uint8_t* _pBuffer;
	uint8_t* _pStart;
	uint8_t* _pEnd;
	uint8_t* _pHead;
	uint8_t* _pTail;
	const uint32_t _size;

};

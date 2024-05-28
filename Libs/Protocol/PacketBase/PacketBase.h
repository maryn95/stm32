#pragma once

#include <stdint.h>

class PacketBase
{
public:
	PacketBase() = default;
	PacketBase(const uint32_t maxLen);
	PacketBase(const uint8_t* pData, const uint32_t maxLen);
	virtual ~PacketBase() {if (_pData) delete [] _pData;}

	uint8_t* getData() const {return _pData;}
	uint32_t getLen() const {return _currentLen;}
	void allocate(const uint32_t maxLen);
	bool reallocate(const uint32_t maxlen);
	bool append(const uint8_t* data, const uint32_t len);
	bool prepend(const uint8_t* data, const uint32_t len);
	void clear() {_currentLen = 0;}

private:
	uint8_t* _pData = nullptr;
	uint32_t _maxLen = 0;
	uint32_t _currentLen = 0;
	
};

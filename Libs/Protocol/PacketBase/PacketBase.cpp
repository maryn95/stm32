#include "PacketBase.h"
#include <string.h>

PacketBase::PacketBase(const uint32_t maxLen)
{
	allocate(maxLen);
}

PacketBase::PacketBase(const uint8_t* pData, const uint32_t maxLen)
{
	if (pData && maxLen > 0)
	{
		_currentLen = _maxLen = maxLen;
		_pData = new uint8_t [_currentLen];
		memcpy(_pData, pData, _currentLen);
	}
}

void PacketBase::allocate(const uint32_t maxLen)
{
	if (!_pData && maxLen > 0)
	{
		_maxLen = maxLen;
		_pData = new uint8_t [_maxLen];
		_currentLen = 0;
	}
}

bool PacketBase::reallocate(const uint32_t maxlen)
{
	if (_maxLen >= maxlen)
		return false;

	if (_pData)
	{
		_maxLen = maxlen;
		uint8_t* temp = new uint8_t [_maxLen];
		memcpy(temp, _pData, _currentLen);
		delete [] _pData;
		_pData = temp;
	}
	else
		allocate(maxlen);

	return true;
}

bool PacketBase::append(const uint8_t* data, const uint32_t len)
{
	if (!data)
		return false;

	allocate(len);

	if (_maxLen < _currentLen + len)
		reallocate(_currentLen + len);

	memcpy(&_pData[_currentLen], data, len);
	_currentLen += len;

	return true;
}

bool PacketBase::prepend(const uint8_t* data, const uint32_t len)
{
	if (!_pData || !data || len == 0)
		return false;

	_maxLen = _currentLen + len;
	uint8_t* temp = new uint8_t [_maxLen];
	memcpy(&temp[0], data, len);
	memcpy(&temp[len], _pData, _currentLen);
	_currentLen = _maxLen;

	delete [] _pData;
	_pData = temp;
	return true;
}

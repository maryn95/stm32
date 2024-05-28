#include "CircularByteBuffer.h"
#include "PacketBase/PacketBase.h"
#include <string.h>

CircularByteBuffer::CircularByteBuffer(const uint32_t size) : _size(size)
{
	_pBuffer = new uint8_t [_size];
	_pStart = &_pBuffer[0];
	_pEnd = &_pBuffer[_size];
	clear();
}

void CircularByteBuffer::clear()
{
	_pHead = _pTail = _pStart;
}

bool CircularByteBuffer::receive(const uint8_t* data, const uint32_t len)
{
	if (!data || len >= _size)
		return false;

	if (_pHead + len < _pEnd)
	{
		memcpy(_pHead, data, len);
		_pHead += len;
	}
	else
	{
		const uint32_t leftLen = _pEnd - _pHead;
		memcpy(_pHead, data, leftLen);
		const uint32_t rightLen = len - leftLen;
		memcpy(_pStart, data + leftLen, rightLen);
		_pHead = &_pBuffer[rightLen];
	}

	return true;
}

bool CircularByteBuffer::getNewData(PacketBase* packet)
{
	if (_pTail == _pHead)
		return false;

	bool result = false;

	if (_pHead > _pTail)
		result = packet->append(_pTail, _pHead - _pTail);
	else
	{
		const uint32_t leftLen = _pEnd - _pTail;
		const uint32_t rightLen = _pHead - _pStart;
		packet->allocate(leftLen + rightLen);
		if (packet->append(_pTail, leftLen))
			result = packet->append(_pStart, rightLen);
	}

	return result;
}

void CircularByteBuffer::moveTail(const uint32_t len)
{
	if (_pHead == _pTail)
		return;

	const uint32_t newDataLen = (_pHead > _pTail) ? (_pHead - _pTail) : \
			(_pEnd - _pTail + _pHead - _pStart);
	if (len > newDataLen)
		return;

	if (_pEnd > _pTail + len)
		_pTail += len;
	else
	{
		const uint32_t leftLen = _pEnd - _pTail;
		const uint32_t rightLen = len - leftLen;
		_pTail = &_pBuffer[rightLen];
	}
}

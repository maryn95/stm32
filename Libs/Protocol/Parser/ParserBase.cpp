#include "ParserBase.h"

ParserBase::ParserBase(IOnPacketReady* packetReady, const uint32_t len, const uint8_t tag) : \
    _packetReady(packetReady), _tag(tag)
{
    _buffer = new CircularByteBuffer(len);
}

ParserBase::~ParserBase()
{
    if (_buffer)
        delete _buffer;
}

void ParserBase::receive(const uint8_t* pData, const uint32_t len)
{
    if (!pData || len == 0)
        return;

    if (!_buffer)
        return;

    _buffer->receive(pData, len);
    PacketBase bufferData = PacketBase();
    if (_buffer->getNewData(&bufferData))
    {
        if (auto parsedLen = parse(&bufferData))
            _buffer->moveTail(parsedLen);
    }
}

void ParserBase::receive(PacketBase* pData)
{
    if (!pData)
        return;

    receive(pData->getData(), pData->getLen());
}

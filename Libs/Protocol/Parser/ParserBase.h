#pragma once

#include "Interface/IOnPacketReady.h"
#include "CircularByteBuffer/CircularByteBuffer.h"
#include "PacketBase/PacketBase.h"
#include <stdint.h>

class ParserBase
{
public:
    ParserBase(IOnPacketReady* packetReady, const uint32_t len, const uint8_t tag);
    virtual ~ParserBase();
    
    void receive(const uint8_t* pData, const uint32_t len);
    void receive(PacketBase* pData);

private:
    virtual uint32_t parse(PacketBase* buffer) = 0;

protected:
    IOnPacketReady* _packetReady = nullptr;
    const uint8_t _tag;

private:
    CircularByteBuffer* _buffer = nullptr;

};

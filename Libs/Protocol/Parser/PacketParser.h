#pragma once

#include "ParserBase.h"

#pragma pack(push, 1)
struct StandardHeader
{
    uint8_t tag;
    uint16_t len;
    uint16_t crc;
};
#pragma pack(pop)

class PacketParser : public ParserBase
{
public:
    PacketParser(IOnPacketReady* packetReady, const uint32_t len, const uint8_t tag/*, const uint32_t maxPacketLen*/) : \
        ParserBase(packetReady, len, tag)/*, _maxPacketLen(maxPacketLen)*/ {reset();}
    ~PacketParser() = default;

private:
    uint32_t parse(PacketBase* buffer) override;
    void reset();

private:
    uint8_t* _pData = nullptr;
    StandardHeader _header = {};
    bool _tagFlag = false;
    bool _headerFlag = false;

//    const uint32_t _maxPacketLen;

};

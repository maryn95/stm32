#include "PacketParser.h"
#include <string.h>
#include <crc.h>

uint32_t PacketParser::parse(PacketBase* buffer)
{
    if (!buffer)
        return 0;

    _pData = buffer->getData();
    const uint32_t len = buffer->getLen();

    if (!_pData || len == 0)
        return 0;

    uint32_t parsedLen = 0;
    while(1)
    {
        if (!_tagFlag)
        {
            const uint32_t cntData = len - parsedLen;
            for (uint32_t i = 0; i < cntData; i++)
            {
                if (*_pData == _tag)
                {
                    _tagFlag = true;
                    break; //for
                }
                _pData++;
                parsedLen++;
            }

            if (!_tagFlag)
                break; //while
        }

        if (!_headerFlag)
        {
            const uint32_t cntData = len - parsedLen;
            const uint32_t headerLen = sizeof(_header);
            if (cntData < headerLen)
                break; //while

            memcpy(reinterpret_cast<uint8_t*>(&_header), _pData, headerLen);

//            if (_header.len > _maxLen)
//            	reset();

            _pData += headerLen;
            parsedLen += headerLen;
            _headerFlag = true;
        }

        const uint32_t cntData = len - parsedLen;
        if (cntData < _header.len)
            break; //while

        const uint16_t packetCrc = crc16(_pData, _header.len);
        if (packetCrc == _header.crc)
        {
            if (_packetReady)
            {
                PacketBase packet = PacketBase(_pData, _header.len);
                _packetReady->onPacketReady(&packet);
            }
        }

        _pData += _header.len;
        parsedLen += _header.len;
        reset();
    }

    return parsedLen;
}

void PacketParser::reset()
{
    _tagFlag = false;
    _headerFlag = false;
    memset(reinterpret_cast<uint8_t*>(&_header), 0, sizeof(_header));
}

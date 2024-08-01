#pragma once

#include "FlashBase.h"

class FlashStorage
{
public:
#pragma pack(push, 1)
    struct StorageParams
    {
        uint32_t pageSize;
        uint32_t startAddress;
        uint32_t startPage;
        uint32_t numPages;
        uint32_t dataSize;
    };
#pragma pack(pop)

public:
    FlashStorage(const FlashBase *flashFunctions, const StorageParams &storage);

private:
    void init();
    bool erase();
    bool write(const uint8_t* data, const uint32_t size);

private:
    static constexpr uint32_t _minBlockSize = sizeof(uint64_t); ///< doubleword
    const FlashBase *_flashFunctions = nullptr;
    const StorageParams _storage;
    uint32_t _blockSize; ///< in doublewords
    uint32_t _endAddress;

    uint32_t _lastAddress;
    uint32_t _addressToWrite;
};

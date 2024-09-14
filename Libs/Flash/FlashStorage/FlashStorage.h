#pragma once

#include "../FlashBase.h"

/*
 * @brief
 * Class provides a storage for the same data structures it flash
 */
class FlashStorage
{
public:
#pragma pack(push, 1)
    struct StorageParams
    {
        uint32_t pageSize; ///< MCU page size
        uint32_t startAddress; ///< storage start address
        uint32_t startPage; ///< storage start page
        uint32_t numPages; ///< number of pages
        uint32_t dataSize; ///< size of a data structure
    };
#pragma pack(pop)

public:
    /*
     * @brief
     * @param flashBase - pointer to instance with flash functions
     * @param storage - storage configuration
     */
    FlashStorage(FlashBase *flashBase, const StorageParams &storage);

private:
    void init();
    bool erase();
    bool write(const uint8_t* data, const uint32_t size);

private:
    static constexpr uint32_t _dWordAlignment = sizeof(uint64_t); ///< doubleword flash addressing
    FlashBase *_flashBase;
    const StorageParams _storage;
    uint32_t _blockSize; ///< size of a data structure aligned by _dWordAlignment
    uint32_t _endAddress; ///< last address to write

    uint32_t _lastAddress; ///< address with last actual data
    uint32_t _addressToWrite; ///< current address to write
};

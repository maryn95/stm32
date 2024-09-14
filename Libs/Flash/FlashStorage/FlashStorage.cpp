#include "FlashStorage.h"

FlashStorage::FlashStorage(FlashBase *flashFunctions, const StorageParams &storage) :
    _flashBase(flashFunctions),
    _storage(storage)
{
    init();
}

void FlashStorage::init()
{
    _blockSize = (_storage.dataSize % _dWordAlignment == 0) ? (_storage.dataSize) : \
        (_storage.dataSize / _dWordAlignment + 1)*_dWordAlignment;
    const uint32_t endPosition = (_storage.pageSize * _storage.numPages - _blockSize)/_blockSize;
    _endAddress = _storage.startAddress + endPosition*_blockSize;

    uint32_t address = _storage.startAddress;
    while (1)
    {
        uint32_t emptyBlocks = 0;
        for (uint32_t i = 0; i < _blockSize/_dWordAlignment; i++)
        {
            uint64_t value = *(uint64_t*)(uint32_t)(address + i*_dWordAlignment);
            if (value != UINT64_MAX)
                break;
            else
                emptyBlocks++;
        }

        if (emptyBlocks == _blockSize/_dWordAlignment)
        {
            if (address == _storage.startAddress)
            {
                _addressToWrite = _storage.startAddress;
                _lastAddress = _storage.startAddress;
                return;
            }
            else
            {
                _addressToWrite = address;
                _lastAddress = (address - _blockSize);
                return;
            }
        }

        address += _blockSize;
        if (address > _endAddress)
        {
            _addressToWrite = _endAddress + _blockSize;
            _lastAddress = _endAddress;
            return;
        }
    }
}

bool FlashStorage::erase()
{
    if (!_flashBase)
        return false;

    if (_flashBase->erasePages(_storage.startPage, _storage.numPages))
    {
        _lastAddress = _storage.startAddress;
        _addressToWrite = _storage.startAddress;
        return true;
    }
    else
        return false;
}

bool FlashStorage::write(const uint8_t* data, const uint32_t size)
{
	if (size > _blockSize)
		return false;

    if (_addressToWrite > _endAddress)
        if (!erase())
            return false;

    if (_flashBase->write(_addressToWrite, data, size))
    {
        _addressToWrite += _blockSize;
        return true;
    }
    else
        return false;
}

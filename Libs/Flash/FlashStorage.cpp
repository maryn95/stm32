#include "FlashStorage.h"

FlashStorage::FlashStorage(const FlashBase *flashFunctions, const StorageParams &storage) :
    _flashFunctions(flashFunctions),
    _storage(storage)
{
    init();
}

void FlashStorage::init()
{
    _blockSize = (_storage.dataSize % _minBlockSize) ? (_storage.dataSize / _minBlockSize) : (_storage.dataSize / _minBlockSize + 1);
    _endAddress = _storage.startAddress + ((_storage.pageSize * _storage.numPages)/(_blockSize * _minBlockSize) - 1) * (_blockSize * _minBlockSize);

    uint32_t address = _storage.startAddress;
    while (1)
    {
        uint32_t emptyBlocks = 0;
        for (uint32_t i = 0; i < _blockSize; i++)
        {
            uint64_t value = *(uint64_t*)(uint32_t)(address + i*_minBlockSize);
            if (value != UINT64_MAX)
                break;
            else
                emptyBlocks++;
        }

        if (emptyBlocks == _blockSize)
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
                _lastAddress = (address - _blockSize * _minBlockSize);
                return;
            }
        }

        address += _blockSize * _minBlockSize;
        if (address > _endAddress)
        {
            _addressToWrite = _endAddress + _blockSize*_minBlockSize;
            _lastAddress = _endAddress;
            return;
        }
    }
}

bool FlashStorage::erase()
{
    if (!_flashFunctions)
        return false;

    if (_flashFunctions->erasePages(_storage.startPage, _storage.numPages))
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
	if (size > _blockSize * _minBlockSize)
		return false;

    if (_addressToWrite > _endAddress)
    {
        if (!eraseFlash())
            return false;
    }

    if (_flashFunctions->write(_addressToWrite, data, size))
    {
        _addressToWrite += _blockSize * _minBlockSize;
        return true;
    }
    else
        return false;
}

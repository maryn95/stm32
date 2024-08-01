#pragma once

#include <stdint.h>

class FlashBase
{
public:
	virtual bool erasePages(const uint32_t startPage, const uint32_t numPages) = 0;
	virtual bool write(const uint32_t address, const uint8_t* pData, const uint32_t size) = 0;
};

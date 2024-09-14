#include "FlashBase.h"

class FlashL : public FlashBase
{
public:
	bool erasePages(const uint32_t startPage, const uint32_t numPages) override;
	bool write(const uint32_t address, const uint8_t* pData, const uint32_t size) override;
};
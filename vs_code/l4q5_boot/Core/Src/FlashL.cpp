#include "FlashL.h"
#include "main.h"
#include "string.h"

bool FlashL::erasePages(const uint32_t startPage, const uint32_t numPages)
{
	if (startPage >= 256)
		return false;

	bool result = false;
	__disable_irq();
	HAL_FLASH_Unlock();

	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Page = startPage;

	if (EraseInitStruct.Page < 128)
		EraseInitStruct.Banks = FLASH_BANK_1;
	else if (EraseInitStruct.Page < 256)
		EraseInitStruct.Banks = FLASH_BANK_2;

	EraseInitStruct.NbPages = numPages;
	result = (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) == HAL_OK ? true : false);

	HAL_FLASH_Lock();
	__enable_irq();
	return result;
}

bool FlashL::write(const uint32_t address, const uint8_t* pData, const uint32_t size)
{
	bool result = true;
    const size_t wordSize = sizeof(uint64_t);
	uint32_t wAddr = address;
	__disable_irq();
	HAL_FLASH_Unlock();

	const uint32_t numWords = (size % wordSize == 0 ? (size / wordSize) : (size / wordSize + 1));
	for (uint32_t i = 0; i < numWords; i++)
	{
		uint64_t dataToWrite = 0xFFFFFFFF;
		memcpy((uint8_t *)&dataToWrite, pData + i * wordSize, wordSize);
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, wAddr, dataToWrite) != HAL_OK)
		{
			result = false;
			break;
		}
		wAddr += wordSize;
	}

	HAL_FLASH_Lock();
	__enable_irq();
	return result;
}
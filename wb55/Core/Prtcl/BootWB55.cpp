/*
 * BootWB55.cpp
 *
 *  Created on: Apr 10, 2024
 *      Author: denis
 */

#include <BootWB55.h>
#include "stm32wbxx_hal.h"
#include <string.h>

bool BootWB55::erasePages(const uint32_t startPage, const uint32_t numPages)
{
	HAL_FLASH_Unlock();

	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct = {.TypeErase = FLASH_TYPEERASE_PAGES,
		.Page = startPage,
		.NbPages = numPages};
	const bool result = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) == HAL_OK ? true : false;

	HAL_FLASH_Lock();
	return result;
}

bool BootWB55::write(const uint32_t address, const uint8_t* pData, const uint32_t size)
{
	static const size_t wordSize = sizeof(uint64_t);
	const uint32_t numDoubleWords = (size % wordSize == 0 ? (size / wordSize) : (size / wordSize + 1));
	uint32_t wrAddress = address;

	bool result = true;
	uint64_t dataToWrite;
	HAL_FLASH_Unlock();
	for (uint32_t i = 0; i < numDoubleWords; i++)
	{
		memcpy((uint8_t *)&dataToWrite, pData + i * wordSize, wordSize);
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, wrAddress, dataToWrite) != HAL_OK)
		{
			result = false;
			break;
		}
		wrAddress += wordSize;
	}

	HAL_FLASH_Lock();
	return result;
}

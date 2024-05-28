/*
 * BootWB55.h
 *
 *  Created on: Apr 10, 2024
 *      Author: denis
 */

#pragma once

#include "Bootloader.h"

class BootWB55 : public Bootloader
{
public:
	explicit BootWB55(const BootloaderInfo& bootInfo, IOnPacketReady* bootCallback = nullptr, uint32_t (*timer)() = nullptr) : \
		Bootloader(bootInfo, bootCallback, timer) {}
	~BootWB55() {}

	bool erasePages(const uint32_t startPage, const uint32_t numPages) override;
	bool write(const uint32_t address, const uint8_t* pData, const uint32_t size) override;

};

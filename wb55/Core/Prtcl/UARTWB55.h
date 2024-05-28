/*
 * UARTWB55.h
 *
 *  Created on: Apr 10, 2024
 *      Author: denis
 */

#pragma once

#include "Physics/UARTDMAPhysics/UARTDMAPhysics.h"

class UARTWB55 : public UARTDMAPhysics
{
public:
	explicit UARTWB55(void (*initPeriphery)(), TransportBase* transport, UART_HandleTypeDef *uart, const uint32_t bufferSize) :
		UARTDMAPhysics(initPeriphery, transport, uart, bufferSize) {receiveDMAStart();}
	virtual ~UARTWB55() {}

	bool isBusy() const override;
	void setBusy(const bool isBusy) = delete;

	bool transmit(const uint8_t* pData, const uint32_t len) override;
	bool transmit(PacketBase* packet) override;

private:
	bool receiveDMAStart();

};

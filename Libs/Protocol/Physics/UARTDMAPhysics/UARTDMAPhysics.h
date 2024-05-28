/*
 * UARTWB55.h
 *
 *  Created on: Apr 10, 2024
 *      Author: denis
 *
 *  Inherited physical layer class for UART
 */

#pragma once

#include <main.h>
#include "Physics/PhysicsBase.h"

class UARTDMAPhysics : public PhysicsBase
{
public:
	explicit UARTDMAPhysics(void (*initPeriphery)(), TransportBase* transport, \
		UART_HandleTypeDef *uart, const uint32_t bufferSize);
	virtual ~UARTDMAPhysics();

//	bool isBusy() const override;
//	void setBusy(const bool isBusy) = delete;

//	bool transmit(const uint8_t* pData, const uint32_t len) override;
//	bool transmit(PacketBase* packet) override;

	bool receive(const uint8_t* pData, const uint32_t len) override;

protected:
	UART_HandleTypeDef* _uart = nullptr;
	uint8_t* _pBuffer = nullptr; ///< pointer to DMA RX circular buffer
	const uint32_t _bufferSize; ///< size of _pBuffer

private:
	#define DMACNDTR _uart->hdmarx->Instance->CNDTR

	uint8_t* _pTail = nullptr; ///< pointer to the tail of _pBuffer
	uint32_t _lastCntValue; ///< last DMACNDTR value

};

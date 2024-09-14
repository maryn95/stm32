#pragma once

#include "Physics/UARTDMAPhysics/UARTDMAPhysics.h"

class PhysicsL : public UARTDMAPhysics
{
public:
  explicit PhysicsL(void (*initPeriphery)(), TransportBase* transport, UART_HandleTypeDef *uart, const uint32_t bufferSize) : 
    UARTDMAPhysics(initPeriphery, transport, uart, bufferSize) {receiveDMAStart();}
  virtual ~PhysicsL() {}

  bool isBusy() const override;
	void setBusy(const bool isBusy) = delete;

  bool transmit(const uint8_t* pData, const uint32_t len) override;
  bool transmit(PacketBase* packet) override;

private:
	bool receiveDMAStart();  
};
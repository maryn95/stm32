/*
 * UARTWB55.h
 *
 *  Created on: Apr 10, 2024
 *      Author: denis
 *
 *  Abstract class for a physical layer
 */

#pragma once

#include <stdint.h>

class TransportBase;
class PacketBase;

class PhysicsBase
{
public:
	explicit PhysicsBase(TransportBase* transport) : _transport(transport) {}
	virtual ~PhysicsBase() {}

	TransportBase* getTransport() const {return _transport;}
	void setTransport(TransportBase* transport) {_transport = transport;}

	bool isConnected() const {return _isConnected;}
	virtual void setConnected(const bool isConnected) {_isConnected = isConnected;}

	virtual bool isBusy() const {return _isBusy;}
	void setBusy(const bool isBusy) {_isBusy = isBusy;}

	virtual bool transmit(const uint8_t* pData, const uint32_t len) = 0;
	virtual bool transmit(PacketBase* packet) = 0;
	virtual bool receive(const uint8_t* pData, const uint32_t len) = 0;

private:
	TransportBase* _transport = nullptr;
	bool _isConnected = false;
	bool _isBusy = false;

};

#pragma once

#include <stdint.h>

class TransportBase;
class PacketBase;
class CircularByteBuffer;

class ApplicationBase
{
public:
	ApplicationBase() = default;
	virtual ~ApplicationBase() = default;

	virtual void onPacketReceived(TransportBase* transport, PacketBase* packet) = 0;
	virtual bool pollData(const TransportBase* transport, const PacketBase* packet) = 0;
	virtual CircularByteBuffer* pollData(const TransportBase* transport) = 0;

};

#include "TransportWB.h"
#include "Physics/PhysicsBase.h"
#include "Application/ApplicationBase.h"
#include "Parser/PacketParser.h"

TransportWB::TransportWB(PhysicsBase* physics, ApplicationBase* application) : \
	TransportBase(physics, application)
{
	_parser = new PacketParser(this, 0x2000, 0x4F);
}

void TransportWB::periodic()
{
	if (!physics() || !application())
		return;

	physics()->receive(nullptr, 0);

	if (physics()->isBusy())
		return;

	PacketBase packet = PacketBase();
	CircularByteBuffer* buffer = application()->pollData(this);
	if (buffer && buffer->getNewData(&packet))
	{
		if (physics()->transmit(&packet))
			buffer->moveTail(packet.getLen());
	}
}

void TransportWB::onPacketReady(PacketBase* packet)
{
	if (!packet)
		return;

	if (application())
		application()->onPacketReceived(this, packet);
}

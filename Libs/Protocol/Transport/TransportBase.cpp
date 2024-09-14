#include "TransportBase.h"

TransportBase::TransportBase(PhysicsBase* physics, ApplicationBase* application) : \
		_physics(physics), _application(application)
{
	if (!_physics->getTransport())
		_physics->setTransport(this);
}

TransportBase::~TransportBase()
{
	if (_parser)
		delete _parser;
}

bool TransportBase::onDataReceived(const uint8_t* pData, const uint32_t len)
{
	if (!_parser || !pData || len == 0)
		return false;

	_parser->receive(pData, len);
	return true;
}

void TransportBase::onPacketReady(PacketBase* packet)
{
	if (!packet || !application())
		return;

	application()->onPacketReceived(this, packet);
}

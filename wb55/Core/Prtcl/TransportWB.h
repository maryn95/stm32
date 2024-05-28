#pragma once

#include "Transport/TransportBase.h"

class ParserBase;

class TransportWB : public TransportBase
{
public:
	explicit TransportWB(PhysicsBase* physics, ApplicationBase* application);
	~TransportWB() = default;

	void periodic() override;
	void onPacketReady(PacketBase* packet) override;

};

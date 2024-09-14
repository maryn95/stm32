#pragma once

#include "Transport/TransportBase.h"

class TransportL : public TransportBase
{
public:
	explicit TransportL(PhysicsBase* physics, ApplicationBase* application);
	~TransportL() = default;

	void periodic() override;
};
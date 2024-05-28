#pragma once

#include <stdint.h>
#include "Interface/IOnPacketReady.h"

class PhysicsBase;
class ApplicationBase;
class ParserBase;

class TransportBase : public IOnPacketReady
{
public:
	explicit TransportBase(PhysicsBase* physics, ApplicationBase* application);
	virtual ~TransportBase();

	PhysicsBase* physics() const {return _physics;}
	void setPhysics(PhysicsBase* physics) {_physics = physics;}

	ApplicationBase* application() const {return _application;}
	void setApplication(ApplicationBase* application) {_application = application;}

	bool onDataReceived(const uint8_t* pData, const uint32_t len);
	virtual void periodic() = 0;

protected:
	ParserBase* _parser = nullptr;

private:
	PhysicsBase* _physics = nullptr;
	ApplicationBase* _application = nullptr;

};

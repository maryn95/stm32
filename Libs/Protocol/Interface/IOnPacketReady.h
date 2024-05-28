#pragma once

class PacketBase;

class IOnPacketReady
{
public:
	virtual void onPacketReady(PacketBase* packet) = 0;
};

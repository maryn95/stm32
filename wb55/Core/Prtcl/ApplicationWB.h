#pragma once

#include "Application/ApplicationBase.h"
#include "Interface/IOnPacketReady.h"

class UARTWB55;
class TransportWB;
class BootWB55;
class CircularByteBuffer;

class ApplicationWB : public ApplicationBase, public IOnPacketReady
{
public:
	ApplicationWB(UARTWB55* uart, const uint32_t bufferSize);
	~ApplicationWB();
/*
 * @brief process input packets from transports
 */
	void onPacketReceived(TransportBase* transport, PacketBase* packet) override;
	bool pollData(const TransportBase* transport, const PacketBase* packet) override {return false;}
	CircularByteBuffer* pollData(const TransportBase* transport) override;
/*
 * @brief process packets from the device logic
 */
	void onPacketReady(PacketBase* packet) override;
/*
 * @brief call periodically in main while
 */
	void periodic();

private:
	bool appendOutputBuffer(PacketBase* packet);

private:
	TransportWB* _transport = nullptr;
	BootWB55* _bootloader = nullptr;
	CircularByteBuffer* _output = nullptr;

};

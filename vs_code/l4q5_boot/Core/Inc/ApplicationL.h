#pragma once

#include "Application/ApplicationBase.h"
#include "Interface/IOnPacketReady.h"
#include "TransportL.h"
#include "FlashL.h"
#include "Bootloader.h"

class ApplicationL : public ApplicationBase, public IOnPacketReady
{
public:
    ApplicationL(PhysicsBase* physics, const uint32_t outBufferSize);
    ~ApplicationL();

    void onPacketReceived(TransportBase* transport, PacketBase* packet) override;
	bool pollData(const TransportBase* transport, const PacketBase* packet) override {return false;}
	CircularByteBuffer* pollData(const TransportBase* transport) override;
    void periodic() override;

    void onPacketReady(PacketBase* packet) override;

private:
    bool appendOutputBuffer(PacketBase* packet);

private:
    TransportL* _transport = nullptr;
    CircularByteBuffer* _outputBuffer = nullptr;
    FlashL* _flash = nullptr;
    Bootloader* _bootloader = nullptr;
};
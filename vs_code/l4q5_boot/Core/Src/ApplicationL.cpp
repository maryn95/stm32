#include "ApplicationL.h"
#include "main.h"
#include "crc.h"

static constexpr uint32_t BOOTLOADER_VERSION = 0x01;
static constexpr uint32_t DEVICE_ID = 0x01;
static constexpr uint32_t VERSION_MAJOR = 0x01;
static constexpr uint32_t VERSION_MINOR = 0x00;
static constexpr uint32_t FIRMWARE_PACKET_SIZE = 0x400;
static constexpr uint32_t FIRMWARE_MAX_SIZE = 0x30000;

__attribute__((section ("DEVICE_INFO"))) static volatile constexpr DeviceInfoFlash DEVICE_INFO = 
{
	DEVICE_INFO_TAG,
	{ BOOTLOADER_VERSION, DEVICE_ID, { VERSION_MAJOR, VERSION_MINOR }, FIRMWARE_PACKET_SIZE, FIRMWARE_MAX_SIZE },
	{'1', '0', '.', '0', '9', '.', '2', '0', '2', '4'}
};

ApplicationL::ApplicationL(PhysicsBase* physics, const uint32_t outBufferSize)
{
    _transport = new TransportL(physics, this);
    _outputBuffer = new CircularByteBuffer(outBufferSize);

    BootloaderInfo bootInfo = {BOOTLOADER_VERSION, DEVICE_ID, 0x1000, 0x00, 0x08000000, 0x05, 0x08005000, 0xFF, 0x080FF000, FIRMWARE_PACKET_SIZE, FIRMWARE_MAX_SIZE};
    _flash = new FlashL();
    _bootloader = new Bootloader(_flash, bootInfo, this, crc16);
}

ApplicationL::~ApplicationL()
{
    if (_transport)
        delete _transport;
    if (_outputBuffer)
        delete _outputBuffer;
}

void ApplicationL::onPacketReceived(TransportBase* transport, PacketBase* packet)
{
    if (!transport || !packet)
        return;

    const uint8_t* data = packet->getData();
    const uint32_t len = packet->getLen();
    if (!data || len == 0)
        return;

    if (_transport == transport)
    {
        switch (data[0])
        {
        case BOOTLOADER_COMMAND:
            if (_bootloader)
            {
                PacketBase bootPacket = PacketBase(data+1, len-1);
                _bootloader->onPacketReceived(&bootPacket);
            }
            break;

        default:
            break;
        }
    }
}

CircularByteBuffer* ApplicationL::pollData(const TransportBase* transport)
{
    if (!transport)
        return nullptr;

    if (transport == _transport)
        return _outputBuffer;
    
    return nullptr;
}

void ApplicationL::periodic()
{
    if (_transport)
        _transport->periodic();
    if (_bootloader)
        _bootloader->periodic();
}

void ApplicationL::onPacketReady(PacketBase* packet)
{
    if (!packet)
        return;

    const uint8_t* data = packet->getData();
    const uint32_t len = packet->getLen();
    if (!data || len == 0)
        return;

    switch (data[0])
    { 
    case BOOTLOADER_COMMAND:
        if (_outputBuffer)
            appendOutputBuffer(packet);
        break;

    default:
        break;
    }
}

bool ApplicationL::appendOutputBuffer(PacketBase* packet)
{
    __disable_irq();
    bool result = false; //TODO header
    result &= _outputBuffer->receive(packet->getData(), packet->getLen());
    __enable_irq();
    return result;
}
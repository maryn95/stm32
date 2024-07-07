#include "ApplicationWB.h"
#include "UARTWB55.h"
#include "TransportWB.h"
#include "BootWB55.h"
#include "CircularByteBuffer/CircularByteBuffer.h"
#include "PacketBase/PacketBase.h"
#include "crc.h"
#include "Parser/PacketParser.h"

static const uint32_t FIRMWARE_ADDRESS = 0x0800A000;

__attribute__((section ("DEVICE_INFO"))) static volatile constexpr DeviceInfoFlash DEVICE_INFO = {
		.tag = DEVICE_INFO_TAG,
		.deviceInfo = {
			.bootloaderVersion = 1,
			.deviceId = 1,
			.version = {
					.major = 1,
					.minor = 2,
			},
			.firmwarePacketSize = 0x400,
			.firmwareMaxSize = 0x3B000,
		},
		.buildTime = {'2', '4', '.', '0', '4', '.', '2', '0', '2', '4'}
};

ApplicationWB::ApplicationWB(UARTWB55* uart, const uint32_t bufferSize)
{
	_transport = new TransportWB(uart, this);
	static const BootloaderInfo flashInfo = {.bootloaderVersion = 1,
			.deviceId = 1,
			.pageSize = 0x1000,
			.bootloaderPage = 0,
			.bootloaderAddress = 0x08000000,
			.firmwarePage = 10,
			.firmwareAddress = FIRMWARE_ADDRESS,
			.statusPage = 30,
			.statusAddress = 0x0801E000,
			.firmwarePacketSize = 0x400,
			.firmwareMaxSize = 0x3B000
	};
	_bootloader = new BootWB55(flashInfo, this, HAL_GetTick);
	_bootloader->setCalcCrc(crc16);
	_output = new CircularByteBuffer(bufferSize);
}

ApplicationWB::~ApplicationWB()
{
	if (_bootloader)
		delete _bootloader;
	if (_output)
		delete _output;
}

void ApplicationWB::onPacketReceived(TransportBase* transport, PacketBase* packet)
{
	if (!transport || !packet)
		return;

	const uint8_t* pData = packet->getData();
	const uint32_t len = packet->getLen();
	if (!pData || len == 0)
		return;

	const uint8_t command = pData[0];
	if (transport == _transport)
	{
		switch (command)
		{
		case BOOTLOADER_COMMAND:
			if (_bootloader)
			{
				PacketBase bootPacket = PacketBase(pData + 1, len - 1);
				_bootloader->onPacketReceived(&bootPacket);
			}
			break;

		default:
			break;
		}
	}
}

CircularByteBuffer* ApplicationWB::pollData(const TransportBase* transport)
{
	if (!transport)
		return nullptr;

	if (transport == _transport)
		return _output;

	return nullptr;
}

//TODO sender
//if (sender == (void*)_bootloader)
void ApplicationWB::onPacketReady(PacketBase* packet)
{
	if (!packet)
		return;

	const uint8_t* pData = packet->getData();
	const uint32_t len = packet->getLen();
	if (!pData || len == 0)
		return;

	const uint8_t command = pData[0];
	switch (command)
	{
	case BOOTLOADER_COMMAND:
		if (_output)
			appendOutputBuffer(packet);
		break;

	default:
		break;
	}
}

bool ApplicationWB::appendOutputBuffer(PacketBase* packet)
{
	__disable_irq();
	StandardHeader header = {.tag = 0x4F,
		.len = static_cast<uint16_t>(packet->getLen()),
		.crc = crc16(packet->getData(), packet->getLen()),
	};
	bool result = _output->receive(reinterpret_cast<const uint8_t*>(&header), sizeof(header));
	result &= _output->receive(packet->getData(), packet->getLen());

	__enable_irq();
	return result;
}

void ApplicationWB::periodic()
{
	if (_transport)
		_transport->periodic();

	if (_bootloader)
		_bootloader->periodic();
}

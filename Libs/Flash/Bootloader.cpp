#include "Bootloader.h"
#include "PacketBase/PacketBase.h"
#include "Interface/IOnPacketReady.h"
#include "main.h"

Bootloader::Bootloader(const BootloaderInfo& bootInfo, IOnPacketReady* bootCallback, uint32_t (*timer)()) : _bootInfo(bootInfo),
	_bootCallback(bootCallback), _timer(timer)
{
	FirmwareStatus* status = readFirmwareStatus(_bootInfo.statusAddress);
	const uint32_t appAddress = SCB->VTOR;
	if (appAddress == bootInfo.firmwareAddress)
	{
		if (checkFirmwareSignature())
			_updateStatus = BootFirmwareOk;
	}
	else
	{
		if (status->status == BootFirmwareOk)
			jumpToApp(_bootInfo.firmwareAddress);

		_deviceInfo = {_bootInfo.bootloaderVersion,
				_bootInfo.deviceId,
				{0, 0},
				_bootInfo.firmwarePacketSize,
				_bootInfo.firmwareMaxSize
		};
		_updateStatus = BootNeedToUpdate;
	}
}

FirmwareStatus* Bootloader::readFirmwareStatus(const uint32_t address)
{
	return reinterpret_cast<FirmwareStatus*>(address);
}

bool Bootloader::checkFirmwareSignature()
{
	const uint8_t tag[10] = DEVICE_INFO_TAG;
	uint32_t i, j, k;
	for (i = 0; i < _bootInfo.firmwareMaxSize - sizeof(DeviceInfoFlash); i++)
	{
		for (j = i, k = 0; k < DEVICE_INFO_TAG_LEN && *reinterpret_cast<uint8_t*>(_bootInfo.firmwareAddress + j) == tag[k]; j++, k++)
			;
		if (k >= DEVICE_INFO_TAG_LEN - 1)
		{
			const DeviceInfoFlash* devInfoFlash = reinterpret_cast<const DeviceInfoFlash*>(_bootInfo.firmwareAddress + i);
			if (devInfoFlash->deviceInfo.deviceId == _bootInfo.deviceId)
			{
				_deviceInfo = devInfoFlash->deviceInfo;
				return true;
			}
		}
	}

	return false;
}

BootloaderErrorEnum Bootloader::checkNewDeviceInfo(const DeviceInfo& deviceInfo)
{
	if (_updateStatus == BootUpdateFinished)
		return BootAlreadyUpdated;

	if (deviceInfo.deviceId != _deviceInfo.deviceId)
		return BootDeviceIdError;

	if (deviceInfo.firmwarePacketSize != _deviceInfo.firmwarePacketSize)
		return BootPacketSizeError;

	if (deviceInfo.firmwareMaxSize != _deviceInfo.firmwareMaxSize)
		return BootFirmwareSizeError;

	if (SCB->VTOR == _bootInfo.firmwareAddress)
	{
		if (deviceInfo.version.major < _deviceInfo.version.major)
			return BootVersionError;

		if (deviceInfo.version.major == _deviceInfo.version.major && deviceInfo.version.minor <= _deviceInfo.version.minor)
			return BootVersionError;

		if (eraseStatusPage() != BootSuccess)
			return BootEraseError;

		jumpToApp(_bootInfo.bootloaderAddress);
	}

	return BootSuccess;
}

BootloaderErrorEnum Bootloader::eraseStatusPage()
{
	return erasePages(_bootInfo.statusPage, 1) ? BootSuccess : BootEraseError;
}

BootloaderErrorEnum Bootloader::writeFirmware(const uint32_t packetNum, const uint8_t* data, const uint32_t size, const uint32_t offset)
{
	if (_updateStatus != BootIsUpdating)
		return BootDeviceInfoError;

	if (packetNum != _currentPacketNum)
		return BootWrongPageNumError;

	if (size != _deviceInfo.firmwarePacketSize)
		return BootPacketSizeError;

	if (offset % size != 0)
		return BootOffsetError;

	if (offset + _deviceInfo.firmwarePacketSize > _deviceInfo.firmwareMaxSize)
		return BootFirmwareSizeError;

//	if (packetNum == 0 && eraseStatusPage() != BootSuccess)
//		return BootEraseError;

	const uint32_t& pageSize = _bootInfo.pageSize;
	if (offset % pageSize == 0)
	{
		const uint32_t pageToErase = _bootInfo.firmwarePage + offset / pageSize;
		const uint32_t numPages = (size % pageSize == 0 ? size / pageSize : size / pageSize + 1);
		if (!erasePages(pageToErase, numPages))
			return BootEraseError;
	}

	const uint32_t address = _bootInfo.firmwareAddress + offset;
	if (!write(address, data, size))
		return BootWriteError;

	return BootSuccess;
}

BootloaderErrorEnum Bootloader::finishUpload(const uint16_t firmwareCrc, const uint32_t firmwareSize)
{
	if (!_calcCrc)
		return BootCrcError;

	if (firmwareCrc != _calcCrc(reinterpret_cast<uint8_t*>(_bootInfo.firmwareAddress), firmwareSize))
		return BootCrcError;

	if (!checkFirmwareSignature())
		return BootSignatureError;

	if (updateStatusPage() != BootSuccess)
		return BootWriteError;

	return BootSuccess;
}

BootloaderErrorEnum Bootloader::updateStatusPage()
{
	FirmwareStatus status = {.status = BootFirmwareOk,
			.updateCounter = 1,
			.updateTimeUnix = 0,
			.crc = 0
	};
	return write(_bootInfo.statusAddress, reinterpret_cast<const uint8_t*>(&status), sizeof(FirmwareStatus)) ? BootSuccess : BootWriteError;
}

BootloaderErrorEnum Bootloader::abortUpload()
{
	if (_updateStatus != BootUpdateFinished)
		_updateStatus = BootNeedToUpdate;
	return BootSuccess;
}

void Bootloader::jumpToApp(uint32_t address)
{
	typedef void (*pFunction)(void);
	pFunction appEntry;
	uint32_t appStack;
	/* Get the application stack pointer (First entry in the application vector table) */
	appStack = (uint32_t)*((volatile uint32_t *)address);

	/* Get the application entry point (Second entry in the application vector table) */
	appEntry = (pFunction)*(volatile uint32_t *)(address + 4);

	/* Reconfigure vector table offset register to watch the application bootloader */
	SCB->VTOR = address;

	/* Set the application stack pointer */
	__set_MSP(appStack);

	appEntry();
}

bool Bootloader::firmwareRequest()
{
	if (!_bootCallback)
		return false;

	PacketBase deviceInfo = PacketBase(1 + sizeof(BootDeviceInfoAnswer_t));
	deviceInfo.append(&BOOTLOADER_COMMAND, sizeof(BOOTLOADER_COMMAND));
	const uint8_t command = BootDeviceInfo;
	deviceInfo.append(&command, sizeof(command));
	deviceInfo.append(reinterpret_cast<const uint8_t*>(&_deviceInfo), sizeof(_deviceInfo));
	_bootCallback->onPacketReady(&deviceInfo);
	return true;
}

void Bootloader::onPacketReceived(PacketBase* packet)
{
	if (!packet)
		return;

	const uint8_t* data = packet->getData();
	const uint32_t len = packet->getLen();
	if (!data || len == 0)
		return;

	PacketBase answer;
	const uint8_t command = data[0];
	switch (command)
	{
	case BootDeviceInfo:
		{
//			const BootDeviceInfoRequest_t* deviceInfo = reinterpret_cast<const BootDeviceInfoRequest_t*>(data);
			answer.allocate(sizeof(BootDeviceInfoAnswer_t));
			const uint8_t command = BootDeviceInfo;
			answer.append(&command, sizeof(command));
			answer.append(reinterpret_cast<const uint8_t*>(&_deviceInfo), sizeof(_deviceInfo));
		}
		break;

	case BootStartUpload:
		{
			const BootStartUploadRequest_t* start = reinterpret_cast<const BootStartUploadRequest_t*>(data);
			answer.allocate(sizeof(BootStartUploadAnswer_t));
			const uint8_t command = BootStartUpload;
			const BootloaderErrorEnum result = checkNewDeviceInfo(start->newDeviceInfo);
			if (result == BootSuccess)
			{
				_currentPacketNum = 0;
				_updateStatus = BootIsUpdating;
//				result = eraseStatusPage();
			}
			answer.append(&command, sizeof(command));
			answer.append(reinterpret_cast<const uint8_t*>(&result), sizeof(result));
		}
		break;

	case BootFirmPacket:
		{
			const BootFirmPacket_t* firmPacket = reinterpret_cast<const BootFirmPacket_t*>(data);
			const uint8_t* fwData = data + sizeof(BootFirmPacket_t) - sizeof(BootFirmPacket_t{}.firmware);
			answer.allocate(sizeof(BootFirmPacketAnswer_t));
			const uint8_t command = BootFirmPacket;
			const BootloaderErrorEnum result = writeFirmware(firmPacket->packetNum, fwData, \
					firmPacket->packetSize, firmPacket->dataOffset);
			answer.append(&command, sizeof(command));
			answer.append(reinterpret_cast<const uint8_t*>(&result), sizeof(result));
			if (result == BootSuccess)
				++_currentPacketNum;
			answer.append(reinterpret_cast<const uint8_t*>(&_currentPacketNum), sizeof(_currentPacketNum));
		}
		break;

	case BootFinishUpload:
		{
			const BootFinishUpload_t* finish = reinterpret_cast<const BootFinishUpload_t*>(data);
			answer.allocate(sizeof(BootFinishUploadAnswer_t));
			const uint8_t command = BootFinishUpload;
			const BootloaderErrorEnum result = finishUpload(finish->firmwareCrc, finish->firmwareSize);
			if (result == BootSuccess)
				_updateStatus = BootUpdateFinished;
			answer.append(&command, sizeof(command));
			answer.append(reinterpret_cast<const uint8_t*>(&result), sizeof(result));
		}
		break;

	case BootAbortUpload:
		abortUpload();
		break;

	case BootSystemReset:
		if (_updateStatus == BootUpdateFinished)
			NVIC_SystemReset();
		break;

	default:
		break;
	}

	if (_bootCallback && answer.getLen() != 0)
	{
		answer.prepend(&BOOTLOADER_COMMAND, sizeof(BOOTLOADER_COMMAND));
		_bootCallback->onPacketReady(&answer);
	}
}

void Bootloader::periodic()
{
	if (_timer)
	{
		static uint32_t localTimer = _timer() + 1000;
		if (_timer() >= localTimer)
		{
			localTimer = _timer() + 1000;
			if (_updateStatus == BootNeedToUpdate)
				firmwareRequest();
		}
	}
}

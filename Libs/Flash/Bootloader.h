#pragma once

#include "FlashBase.h"

#define DEVICE_INFO_TAG {0x11, 0x07, 0x5A, 0x78, 0x3B, 0x02, 0x96, 0xCC, 0xF0, 0xF0}
static constexpr uint8_t DEVICE_INFO_TAG_LEN = 10;
static constexpr uint8_t BOOTLOADER_COMMAND = 0x01;

#pragma pack(push, 1)
typedef struct
{
	uint16_t major;
	uint16_t minor;
} FirmwareVersion;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
	uint16_t bootloaderVersion;
	uint32_t deviceId;
	uint32_t pageSize;
	uint32_t bootloaderPage;
	uint32_t bootloaderAddress;
	uint32_t firmwarePage;
	uint32_t firmwareAddress;
	uint32_t statusPage;
	uint32_t statusAddress;
	uint32_t firmwarePacketSize;
	uint32_t firmwareMaxSize;
} BootloaderInfo;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
	uint16_t bootloaderVersion;
	uint32_t deviceId;
	FirmwareVersion version;
	uint32_t firmwarePacketSize;
	uint32_t firmwareMaxSize;
} DeviceInfo;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
	uint8_t tag[10];
	DeviceInfo deviceInfo;
	char buildTime[10];
} DeviceInfoFlash;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
	uint16_t status;
	uint16_t updateCounter;
	uint64_t updateTimeUnix;
	uint16_t crc;
} FirmwareStatus;
#pragma pack(pop)

enum BootloaderStatus : uint16_t
{
	BootNotInit = 0x00,
	BootFirmwareOk = 0x01,
	BootNeedToUpdate = 0x02,
	BootIsUpdating = 0x03,
	BootUpdateFinished = 0x04,

};

enum BootloaderErrorEnum : uint8_t
{
	BootSuccess = 0x00,
	BootPacketSizeError = 0x01,
	BootOffsetError = 0x02,
	BootEraseError = 0x03,
	BootWrongPageNumError = 0x04,
	BootFirmwareSizeError = 0x05,
	BootWriteError = 0x06,
	BootCrcError = 0x07,

	BootDeviceIdError = 0x20,
	BootVersionError = 0x21,
	BootDeviceInfoError = 0x22,
	BootSignatureError = 0x23,
	BootAlreadyUpdated = 0x24,

};

enum BootloaderCommandsEnum : uint8_t
{
	BootDeviceInfo = 0x01,
	BootStartUpload = 0x02,
	BootFirmPacket = 0x03,
	BootFinishUpload = 0x04,
	BootAbortUpload = 0x05,
	BootSystemReset = 0x06,

};

/*
 * @brief request for a device info
 */
#pragma pack(push, 1)
typedef struct
{
	BootloaderCommandsEnum command; ///< = BootDevInfoReq
} BootDeviceInfoRequest_t;
#pragma pack(pop)

/*
 * @brief answer for BootDeviceInfoRequest_t
 */
#pragma pack(push, 1)
typedef struct
{
	BootloaderCommandsEnum command; ///< = BootDevInfoReq
	DeviceInfo deviceInfo;
} BootDeviceInfoAnswer_t;
#pragma pack(pop)

/*
 * @brief start upload structure
 */
#pragma pack(push, 1)
typedef struct
{
	BootloaderCommandsEnum command; ///< = BootStartUpload
	DeviceInfo newDeviceInfo;
} BootStartUploadRequest_t;
#pragma pack(pop)

/*
 * @brief answer for BootStartUpload_t
 */
#pragma pack(push, 1)
typedef struct
{
	BootloaderCommandsEnum command; ///< = BootStartUpload
	BootloaderErrorEnum result;
} BootStartUploadAnswer_t;
#pragma pack(pop)

/*
 * @brief firmware data packet
 */
#pragma pack(push, 1)
typedef struct
{
	BootloaderCommandsEnum command; ///< = BootFirmPacket
    uint32_t packetNum;
    uint32_t packetSize;
    uint32_t dataOffset;
    uint8_t *firmware;
} BootFirmPacket_t;
#pragma pack(pop)

/*
 * @brief answer for BootFirmPacket_t
 */
#pragma pack(push, 1)
typedef struct
{
	BootloaderCommandsEnum command; ///< = BootFirmPacket
	BootloaderErrorEnum result;
    uint32_t packetNum;
} BootFirmPacketAnswer_t;
#pragma pack(pop)

/*
 * @brief finish upload structure
 */
#pragma pack(push, 1)
typedef struct
{
	BootloaderCommandsEnum command; ///< = BootFinishUpload
	uint32_t firmwareSize;
    uint16_t firmwareCrc;
} BootFinishUpload_t;
#pragma pack(pop)

/*
 * @brief answer for BootFinishUpload_t
 */
#pragma pack(push, 1)
typedef struct
{
	BootloaderCommandsEnum command; ///< = BootFinishUpload
	BootloaderErrorEnum result;
} BootFinishUploadAnswer_t;
#pragma pack(pop)

class PacketBase;
class IOnPacketReady;

class Bootloader : public FlashBase
{
public:
	explicit Bootloader(const BootloaderInfo& bootInfo, IOnPacketReady* bootCallback = nullptr, uint32_t (*timer)() = nullptr);
	virtual ~Bootloader() = default;

	void setCalcCrc(uint16_t (*calcCrc)(uint8_t* data, uint32_t size)) {_calcCrc = calcCrc;}
	void setBootCallback(IOnPacketReady* bootCallback) {_bootCallback = bootCallback;}
	void onPacketReceived(PacketBase* packet);
	void periodic();

private:
	FirmwareStatus* readFirmwareStatus(const uint32_t address);
	bool checkFirmwareSignature();
	BootloaderErrorEnum checkNewDeviceInfo(const DeviceInfo& deviceInfo);
	BootloaderErrorEnum eraseStatusPage();
	BootloaderErrorEnum writeFirmware(const uint32_t packetNum, const uint8_t* data, const uint32_t size, const uint32_t offset);
	BootloaderErrorEnum finishUpload(const uint16_t firmwareCrc, const uint32_t firmwareSize);
	BootloaderErrorEnum updateStatusPage();
	BootloaderErrorEnum abortUpload();
	void jumpToApp(uint32_t address);
	bool firmwareRequest();

private:
	const BootloaderInfo _bootInfo;
	DeviceInfo _deviceInfo;

	BootloaderStatus _updateStatus = BootNotInit;
	uint32_t _currentPacketNum = 0;

	uint16_t (*_calcCrc)(uint8_t* data, uint32_t size) = nullptr;
	IOnPacketReady* _bootCallback = nullptr;
	uint32_t (*_timer)() = nullptr;

};

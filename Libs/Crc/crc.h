#ifndef __CRC_H
#define __CRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint16_t crc16(uint8_t *pcBlock, uint32_t len);
uint8_t crc8(uint8_t *pcBlock, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif

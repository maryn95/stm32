#include "PhysicsL.h"

bool PhysicsL::isBusy() const
{
    return _uart->gState != HAL_UART_STATE_READY ? true : false;
}

bool PhysicsL::transmit(const uint8_t *pData, const uint32_t len)
{
    if (!pData || len == 0 || isBusy())
        return false;

    return (HAL_UART_Transmit_DMA(_uart, pData, len) == HAL_OK) ? true : false;
}

bool PhysicsL::transmit(PacketBase* packet)
{
	if (!packet)
		return false;

	return transmit(packet->getData(), packet->getLen());
}

bool PhysicsL::receiveDMAStart()
{
//	return (HAL_DMA_Start_IT(_uart->hdmarx, (uint32_t)&(_uart->Instance->RDR), (uint32_t)&_pBuffer[0], _bufferSize)
//		== HAL_OK ? true : false);
	const bool result = HAL_DMA_Start(_uart->hdmarx, (uint32_t)&(_uart->Instance->RDR), \
		(uint32_t)_pBuffer, _bufferSize) == HAL_OK ? true : false;
	if (result)
	    _uart->Instance->CR3 |= USART_CR3_DMAR;
	return result;
}

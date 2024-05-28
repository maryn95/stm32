#include "UARTDMAPhysics.h"
#include "Transport/TransportBase.h"

UARTDMAPhysics::UARTDMAPhysics(void (*initPeriphery)(), TransportBase* transport, \
		UART_HandleTypeDef *uart, const uint32_t bufferSize) :
		PhysicsBase(transport),
		_uart(uart),
		_bufferSize(bufferSize)
{
	_pBuffer = new uint8_t[_bufferSize];
	_pTail = &_pBuffer[0];
	_lastCntValue = _bufferSize;
	if (initPeriphery)
		initPeriphery();
}

UARTDMAPhysics::~UARTDMAPhysics()
{
	if (_pBuffer)
		delete [] _pBuffer;
}

//bool UARTDMAPhysics::isBusy() const
//{
//	return _uart->gState != HAL_UART_STATE_READY ? true : false;
//}

//bool UARTDMAPhysics::transmit(const uint8_t* pData, const uint32_t len)
//{
//	if (!pData || len == 0)
//		return false;
//
//	bool result = false;
//	if (!_uart->hdmatx)
//		result = (HAL_UART_Transmit(_uart, pData, len, 100) == HAL_OK) ? true : false;
//	else if (!isBusy())
//		result = (HAL_UART_Transmit_DMA(_uart, pData, len) == HAL_OK) ? true : false;
//
//	return result;
//}
//
//bool UARTDMAPhysics::transmit(PacketBase* packet)
//{
//	if (!packet)
//		return false;
//
//	return transmit(packet->getData(), packet->getLen());
//}

bool UARTDMAPhysics::receive(const uint8_t* pData, const uint32_t len)
{
	if (DMACNDTR != _lastCntValue)
	{
		const uint32_t _cntrData = DMACNDTR;
		if (_cntrData <= _lastCntValue)
		{
			TransportBase* transport = getTransport();
			if (transport)
				transport->onDataReceived(_pTail, _lastCntValue - _cntrData);
		}
		else //dma buffer is looped
		{
			TransportBase* transport = getTransport();
			if (transport)
			{
				transport->onDataReceived(_pTail, _lastCntValue);
				transport->onDataReceived(&_pBuffer[0], _bufferSize - _cntrData);
			}
		}

		_pTail = &_pBuffer[_bufferSize - _cntrData];
		_lastCntValue = _cntrData;
		return true;
	}
	else
		return false;
}

#include "stm32l4xx_hal.h"
#include <gtest/gtest.h>

class GpioTest : public ::testing::Test
{
public:
    GPIO_TypeDef _gpio = {};
};

TEST_F(GpioTest, GpioInit)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(&_gpio, &GPIO_InitStruct);

    ASSERT_EQ(_gpio.MODER, GPIO_MODE_OUTPUT_PP);
    ASSERT_EQ(_gpio.OSPEEDR, GPIO_SPEED_FREQ_HIGH);
}

TEST_F(GpioTest, GpioWrite)
{
    HAL_GPIO_WritePin(&_gpio, GPIO_PIN_0, GPIO_PIN_SET);
    ASSERT_EQ(_gpio.BSRR, GPIO_PIN_0);
    HAL_GPIO_WritePin(&_gpio, GPIO_PIN_10, GPIO_PIN_SET);
    ASSERT_EQ(_gpio.BSRR, GPIO_PIN_10);
}

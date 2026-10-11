#pragma once

extern "C" {
    #include "stm32f3xx_hal.h"
}

const uint16_t DAC_MAX_VALUE = 4095;

const double SENSOR_VDD = 3.3;
const double NO_CURRENT_OUTPUT = 1.65;
const double SENSOR_MAX_CURRENT = 32.2;

COMP_HandleTypeDef hcomp4;
DAC_HandleTypeDef hdac1;

static void gpio_init() {
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // COMP4_OUT (PB1)
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_GPCOMP4; 
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void comp4_init() {
    hcomp4.Instance = COMP4;
    hcomp4.Init.InvertingInput = COMP_INVERTINGINPUT_DAC1_CH2;
    hcomp4.Init.NonInvertingInput = COMP_NONINVERTINGINPUT_IO1;
    hcomp4.Init.Output = COMP_OUTPUT_NONE;
    hcomp4.Init.OutputPol = COMP_OUTPUTPOL_INVERTED;
    hcomp4.Init.BlankingSrce = COMP_BLANKINGSRCE_NONE;
    hcomp4.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
    if (HAL_COMP_Init(&hcomp4) != HAL_OK) { Error_Handler(); }

    HAL_COMP_Start(&hcomp4);
}

static void dac1_init() {
    __HAL_RCC_DAC1_CLK_ENABLE();

    hdac1.Instance = DAC1;
    if (HAL_DAC_Init(&hdac1) != HAL_OK) { Error_Handler(); }

    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK) { Error_Handler(); }

    HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
}

void current_limit_init() {
    gpio_init();
    comp4_init();
    dac1_init();
}

uint16_t voltage_to_dac_value(double voltage) {
  return (voltage / 3.3) * DAC_MAX_VALUE;
}

double current_to_voltage(double current) {
  return (((SENSOR_VDD - 0.1) - NO_CURRENT_OUTPUT) / SENSOR_MAX_CURRENT) * current + NO_CURRENT_OUTPUT;
}

void set_current_limit(double current) {
  uint16_t value = voltage_to_dac_value(current_to_voltage(current));
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, value);
}

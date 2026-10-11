#pragma once

extern "C" {
  #include "stm32f3xx_hal_can.h"
}

CAN_HandleTypeDef hcan = {};
CAN_RxHeaderTypeDef rxHeader  = {};
uint8_t rxData[8] = {0};

extern "C" void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle) {
    if (canHandle->Instance != CAN) return;

    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {};
    gpio.Pin              = GPIO_PIN_11 | GPIO_PIN_12;
    gpio.Mode             = GPIO_MODE_AF_PP;
    gpio.Pull             = GPIO_NOPULL;
    gpio.Speed            = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate        = GPIO_AF9_CAN;

    HAL_GPIO_Init(GPIOA, &gpio);
}

void can_init() {
    hcan.Instance                  = CAN;
    hcan.Init.Prescaler            = 2;
    hcan.Init.Mode                 = CAN_MODE_NORMAL;
    hcan.Init.SyncJumpWidth        = CAN_SJW_1TQ;
    hcan.Init.TimeSeg1             = CAN_BS1_11TQ;
    hcan.Init.TimeSeg2             = CAN_BS2_4TQ;
    hcan.Init.TimeTriggeredMode    = DISABLE;
    hcan.Init.AutoBusOff           = ENABLE;
    hcan.Init.AutoWakeUp           = DISABLE;
    hcan.Init.AutoRetransmission   = ENABLE;
    hcan.Init.ReceiveFifoLocked    = DISABLE;
    hcan.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&hcan) != HAL_OK) {
        Serial.println("CAN init failed");
        while (true)
            delay(1000);
    }

    CAN_FilterTypeDef filter    = {};
    filter.FilterBank           = 0;
    filter.FilterMode           = CAN_FILTERMODE_IDMASK;
    filter.FilterScale          = CAN_FILTERSCALE_32BIT;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterActivation     = ENABLE;

    if (HAL_CAN_ConfigFilter(&hcan, &filter) != HAL_OK || HAL_CAN_Start(&hcan) != HAL_OK) {
        Serial.println("CAN start failed");
        while (true)
            delay(1000);
    }

    Serial.println("CAN started: 1 Mbps");
}
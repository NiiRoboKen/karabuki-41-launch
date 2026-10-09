#include <Arduino.h>
#include <Pid.h>
#include <message.h>
#include "current_limit.h"
#include "incremental_encoder.hpp"
#include "motor.hpp"
#include "bxcan.h"

const uint32_t UART_BAUD = 115200;

const uint8_t ENC_A_PIN = PA6;
const uint8_t ENC_B_PIN = PA7;
const uint8_t DEBUG_PIN = PA10;
const uint8_t LAUNCH_PWM_PIN = PA8;
const uint8_t LAUNCH_DIR_PIN = PB0;
const uint8_t LOAD_PWM_PIN = PA9;
const uint8_t LOAD_DIR_PIN = PA1;
const uint8_t LIM_SW_PIN = PB4;
// メモ（裏で自動で動くのでこれらピンを操作する必要なし）
// const uint8_t COMP_IN_PIN = PB0;
// const uint8_t COMP_OUT_PIN = PB1;

const uint32_t ENC_CPR = 4096;
const uint16_t CAN_TX_ID_TARGET_REACHED = 0x102;
const double   TARGET_RPM_TOLERANCE     = 100.0;
const uint8_t  PWM_LIMIT      = 255;
const uint16_t INTEGRAL_LIMIT = 1000;

PID pid(
  1.0,                            // kp
  0.0,                            // ki
  0.0,                            // kd
  -PWM_LIMIT, PWM_LIMIT,          // 出力制限
  -INTEGRAL_LIMIT, INTEGRAL_LIMIT // 積分制限
);
HardwareTimer encoderTimer(TIM3);
IncrementalEncoder enc(encoderTimer.getHandle(), ENC_A_PIN, ENC_B_PIN);
Motor launch(LAUNCH_DIR_PIN, LAUNCH_PWM_PIN);
Motor load(LOAD_DIR_PIN, LOAD_PWM_PIN);

// 目標RPM
double target_rpm = -4000.0;
int64_t       prev_count = 0;
unsigned long prev_ms    = 0;
bool load_finished = false;
bool launch_started = false;
bool roller_started = false;
bool roller_stop    = false;
uint32_t last_printed  = millis();
uint32_t last_can_sent = 0;

void can_send_target_reached() {
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0) return;
    CAN_TxHeaderTypeDef tx_header = {};
    tx_header.StdId               = CAN_TX_ID_TARGET_REACHED;
    tx_header.IDE                 = CAN_ID_STD;
    tx_header.RTR                 = CAN_RTR_DATA;
    tx_header.DLC                 = 0;
    uint8_t  data[8] = {};
    uint32_t mailbox = 0;
    HAL_CAN_AddTxMessage(&hcan, &tx_header, data, &mailbox);
}

void can_receive_command() {
  while (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) > 0) {
    if (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK) { return; }
    digitalWrite(DEBUG_PIN, !digitalRead(DEBUG_PIN));
    if (rxHeader.IDE != CAN_ID_STD) { continue; }
    Serial.println(rxHeader.StdId);
    switch (rxHeader.StdId) {
      case CAN_CMD_LAUNCH_START:
        launch_started = true;
        Serial.println("Launch started");
        break;
      case CAN_CMD_LAUNCH_ROLLER:
        roller_started = true;
        Serial.println("Roller started");
        break;
      case CAN_CMD_STOP_ROLLER:
        launch_started = false;
        Serial.println("Launch stop");
        break;
      default:
        break;
    }
  }
}

void setup() {
  Serial.begin(UART_BAUD);

  can_init();
  current_limit_init();

  pinMode(LAUNCH_DIR_PIN, OUTPUT);
  pinMode(LAUNCH_PWM_PIN, OUTPUT);
  pinMode(LOAD_DIR_PIN, OUTPUT);
  pinMode(LOAD_PWM_PIN, OUTPUT);
  pinMode(LIM_SW_PIN, INPUT);

  if (!enc.begin()) {
    Serial.println("Encoder initialization failed");
    while (true) {}
  }

  set_current_limit(20.0);
  enc.clear();
  pid.reset(0.0);

  prev_count = enc.getCount();
  prev_ms    = millis();
}

void loop() {
  can_receive_command();
  const unsigned long now_ms = millis();
  const double        dt     = (now_ms - prev_ms) / 1000.0;

  if (dt < 0.01) return; // 10ms周期程度
  prev_ms = now_ms;

  const int64_t now_count   = enc.getCount();
  const int64_t delta_count = now_count - prev_count;
  prev_count                = now_count;

  // counts -> RPM
  const double measured_rpm = (delta_count / static_cast<double>(ENC_CPR)) / dt * 60.0;

  bool target_reached = false;

  if (launch_started) {
    const double control = pid.update(target_rpm, measured_rpm, dt);
    launch.run(static_cast<int>(control));

    target_reached = std::fabs(measured_rpm - target_rpm) <= TARGET_RPM_TOLERANCE;
  } else {
    launch.run(0);
  }

  if (launch_started) {
      if (!load_finished) {
        while (digitalRead(LIM_SW_PIN) == 0) {
        load.run(100);
      }

      if (digitalRead(LIM_SW_PIN) == 0) {
        load.run(0);
        load_finished  = true;
        launch_started = false;
        Serial.println("Load limit reached");
      } else {
        load.run(100);
      }
    }
  }

  // if (target_reached && now_ms - last_can_sent >= 200) {
  if (now_ms - last_can_sent >= 200) {
    can_send_target_reached();
    last_can_sent = now_ms;
  }

  if (millis() - last_printed >= 200) {
    Serial.print(digitalRead(LIM_SW_PIN));
    Serial.print("rpm: ");
    Serial.println(measured_rpm);
    last_printed = millis();
  }
}

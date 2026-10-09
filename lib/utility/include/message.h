#pragma once
#include "peer_link.h"
#include <cstdint>

enum class MessageType {
    // ロボへの命令
    Stop       = 0x00,
    GamePadUse = 0x01,
    TabletUse  = 0x02,
    Position   = 0x03,
    Reboot     = 0x04,
    //     gamepadData
    Gamepad = 0x11,
    PS4Data = 0x12,
    //     ロボ状態
    RobotState = 0x21,
    //     ベルト
    UnloadChanber     = 0x29,
    LoadChamber       = 0x30,
    UnloadMag         = 0x31,
    LoadMag           = 0x32,
    BeltDesk          = 0x33,
    BeltBucket_Low    = 0x34,
    BeltBucket_Middle = 0x35,
    BeltBucket_High   = 0x36,
    BeltFlag          = 0x37,
    BeltElevation     = 0x38,
    BeltLaunch        = 0x39,
    //     ローラ
    RollerStart  = 0x41,
    RollerLaunch = 0x42,
    RollerStop   = 0x43,
    //     バケツ回収
    BucketLow     = 0x51,
    BucketMiddle  = 0x52,
    BucketHigh    = 0x53,
    BucketRelease = 0x54,
    // PS4コントローラーへの命令
    //     振動
    PS4SetRumble = 0xA1,
    //     LED
    PS4SetLED = 0xA2,
};

struct JoyStick {
        int8_t x;
        int8_t y;
};

union Buttons {
        uint16_t raw;
        struct {
                uint16_t north : 1;
                uint16_t east : 1;
                uint16_t south : 1;
                uint16_t west : 1;
                uint16_t joystick_left : 1;
                uint16_t joystick_right : 1;
                uint16_t shoulder_left : 1;
                uint16_t shoulder_right : 1;
                uint16_t trigger_left : 1;
                uint16_t trigger_right : 1;
                uint16_t start : 1;
                uint16_t select : 1;
                uint16_t __reserved : 4;
        } bits;
};

enum class Dpad : uint8_t {
    Up        = 0,
    RightUp   = 1,
    Right     = 2,
    RightDown = 3,
    Down      = 4,
    LeftDown  = 5,
    Left      = 6,
    LeftUp    = 7,
    Neutral   = 8
};

struct __attribute__((packed)) GamepadData {
        struct JoyStick joystick_left;
        struct JoyStick joystick_right;
        uint8_t         trigger_left;
        uint8_t         trigger_right;
        union Buttons   buttons;
        enum Dpad       dpad;
};

struct __attribute__((packed)) PS4Data {
        union {
                uint8_t raw;
                struct {
                        uint8_t touchpad : 1;
                        uint8_t ps : 1;
                        uint8_t __reserved : 6;
                } bits;
        } buttons;
};

struct __attribute__((packed)) PS4RumbleData {
        uint8_t  small_rumble;
        uint8_t  big_rumble;
        uint16_t duration; // 単位はmilli second
};

struct __attribute__((packed)) PS4LedData {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        // 以下が両方0なら常時点灯
        uint8_t flash_on;  // オン時間
        uint8_t flash_off; // オフ時間
};

struct __attribute__((packed)) TabletData_Pos {
        int16_t x;
        int16_t y;
        int16_t deg;
};

struct __attribute__((packed)) StateData {
        bool gamepad_used;
        bool roller_reach;
};

struct __attribute__((packed)) BeltData {
        bool     load_chamber;
        bool     unload_mag;
        bool     load_mag;
        bool     launch;
        bool     unload_chamber;
        bool     elevation_change;
        uint8_t  elevation_pos;
        uint16_t acc;
};

struct __attribute__((packed)) RollerData {
        bool     acc_start;
        bool     launch;
        bool     stop;
        uint16_t acc_value;
};

// 0x00 緊急停止
// 0x30 リセット
constexpr uint8_t CAN_CMD_STOP                  = 0x00;
constexpr uint8_t CAN_CMD_LOAD_CHAMBER          = 0x01;
constexpr uint8_t CAN_CMD_UNLOAD_MAG            = 0x02;
constexpr uint8_t CAN_CMD_LOAD_MAG              = 0x03;
constexpr uint8_t CAN_CMD_LAUNCH_BELT           = 0x04;
constexpr uint8_t CAN_CMD_LAUNCH_ELEVATION_BELT = 0x11;
constexpr uint8_t CAN_CMD_UNLOAD_CHAMBER        = 0x12;

constexpr uint8_t CAN_CMD_STOP_ROLLER   = 0x20;
constexpr uint8_t CAN_CMD_LAUNCH_START  = 0x21;
constexpr uint8_t CAN_CMD_LAUNCH_ROLLER = 0x22;

constexpr uint8_t CAN_RESET = 0x30;

constexpr uint8_t   WIFI_CHANNEL   = 14;
constexpr peer_id_t SWERVE_S3_ID   = 0x10;
constexpr peer_id_t TABLET_ESP_ID  = 0x11;
constexpr peer_id_t Gamepad_ESP_ID = 0x12;

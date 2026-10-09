# robocon_2026_utility

## 通信メッセージ定義

`MessageType` と対応するデータ構造を定義します。

## メッセージ一覧

| MessageType | 値 | 送信元 → 送信先 | データ | 概要 |
|---|---:|---|---|---|
| `GamePadUse` | `0x01` | Tablet → ESP | なし | ゲームパッド操作へ切り替え |
| `TabletUse` | `0x02` | Tablet → ESP | なし | タブレット操作へ切り替え |
| `Position` | `0x03` | Tablet ⇔ ESP | `TabletData_Pos` | 目標位置または現在位置 |
| `Reboot` | `0x04` |  |  | 再起動 |
| `Gamepad` | `0x11` | GamePad → ESP | `GamepadData` | ゲームパッド入力 |
| `RobotState` | `0x21` | ESP → Tablet | `StateData` | ロボット状態(今の所使わないかも) |
| `BeltLoad` | `0x30` | Tablet → ESP | なし | ベルトロード |
| `BeltReload` | `0x31` | Tablet → ESP | なし | ベルトリロード |
| `BeltReloadFinish` | `0x32` | Tablet → ESP | なし | ベルトリロード完了 |
| `BeltDesk` | `0x33` | Tablet → ESP | なし | 机の位置に角度を変える |
| `BeltBucket_Low` | `0x34` | Tablet → ESP | なし | バケツ低の位置に角度を変える |
| `BeltBucket_Middle` | `0x35` | Tablet → ESP | なし | バケツ中位置に角度を変える  |
| `BeltBucket_High` | `0x36` | Tablet → ESP | なし | バケツ高位置に角度を変える  |
| `BeltFlag` | `0x37` | Tablet → ESP | なし | 旗位置に角度を変える  |
| `BeltElevation` | `0x38` |  |  | (今の所使わない) |
| `BeltLaunch` | `0x39` | Tablet → ESP | `uint16_t 加速距離` | ベルト発射。加速度値を含む |
| `RollerStart` | `0x41` |  |  | 加速開始 |
| `RollerLaunch` | `0x42` |  |  | 発射 |
| `BucketLow` | `0x51` |  |  | バケツ回収 低 |
| `BucketMiddle` | `0x52` |  |  | バケツ回収 中 |
| `BucketHigh` | `0x53` |  |  | バケツ回収 高 |
| `BucketRelease` | `0x54` |  |  | 雑巾を落とす |
| `FloorOn` | `0x61` |  |  | 床回収 on |
| `FloorOff` | `0x62` |  |  | 床回収off |



### `TabletData_Pos`

位置・姿勢情報です。

| メンバー | 型 | 内容 |
|---|---|---|
| `x` | `int16_t` | X座標 |
| `y` | `int16_t` | Y座標 |
| `deg` | `int16_t` | 角度 |

Tablet から受信した場合は目標位置として使用し、ESP から送信する場合は現在位置として使用します。

<!-- ### `StateData`

ESP から Tablet へ送信するロボット状態です。

| メンバー | 型 | 内容 |
|---|---|---|
| `gamepad_used` | `bool` | ゲームパッド操作中 |
| `load_belt` | `bool` | ロード状態 |
| `reload_belt` | `bool` | リロード状態 |
| `reload_finish_belt` | `bool` | リロード完了状態 |
| `launch_belt` | `bool` | 発射状態 |
| `launch_pos_belt` | `uint8_t` | 発射位置 |
| `acc_pos_belt` | `uint16_t` | 加速度位置 | -->

### `BeltData`

ベルト制御用の内部データです。

| メンバー | 型 | 内容 |
|---|---|---|
| `load_belt` | `bool` | ロード要求 |
| `reload_belt` | `bool` | リロード要求 |
| `reload_finish_belt` | `bool` | リロード完了要求 |
| `launch` | `bool` | 発射要求 |
| `elevation_change` | `bool` | 高さ変更要求 |
| `elevation_pos` | `uint8_t` | 高さ位置 |
| `acc` | `uint16_t` | 加速度値 |

## CANコマンド

| 定数 | 値 | 内容 |
|---|---:|---|
| `CAN_CMD_LOAD_BELT` | `0x01` | ベルトロード |
| `CAN_CMD_RELOAD_BELT` | `0x02` | ベルトリロード |
| `CAN_CMD_RELOAD_FINISH_BELT` | `0x03` | リロード完了 |
| `CAN_CMD_LAUNCH_BELT` | `0x04` | ベルト発射 |
| `CAN_CMD_LAUNCH_ELEVATION_BELT` | `0x11` | 発射高さ変更 |

## 通信設定

| 項目 | 値 |
|---|---:|
| Wi-Fiチャンネル | `14` |
| Swerve ESP ID | `0x13` |
| Tablet ESP ID | `0x14` |
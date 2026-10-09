# PID_library

このライブラリを使う際は、
```
unsigned long now = micros();

if (now - last < CONTROL_CYCLE) return;
double dt = (now - last) * 1.e-6;
last      = now;
```
という感じで、dtを取得してください。

これは実測dtの方法で、dtを固定化したい場合(速度制御のときとか)は
```
unsigned long now = micros();

if (now - last < CONTROL_CYCLE) return;

last += CONTROL_CYCLE; // 周期固定

double dt = CONTROL_CYCLE * 1e-6;
```
にしてください。
dtは[s]で与えてください

## ライブラリの使い方
libにcd して、git clone して、
```
#include "AnglePID.h"
```
のようにしたら動きます。


## RawPID
基本的なPIDです。
```
Pid hoge(double kp,double ki,double kd);
```
で値を設定して呼び出します。
```
setTarget(double target);
```
で目標値を指定し、
```
update(double now_value,double dt);
```
を呼ぶことで差分を取りその差分に応じた出力が返ってきます。
正直ロボットの制御には至らぬ点がありすぎて使いたくないですが動きます。

## AnglePID
角度に特化したPIDです。正規化しているので、最短で回ってくれます。
出力の制限と、I項の制限ができます。
rangeに一周の大きさ入れてください。

## Pid
これは、位置に特化したPIDです。
タイヤを何メートル動かすとかの処理がしやすいです。
出力の制限と、I項の制限ができます。
位置制御系は、主にP項だけでいいです正直。
ほんのりD項かな、Iはまじ少なめでいいかも

## IncrementalPID
これはモーター速度を制御するのに特化したPIDです。
このライブラリ唯一の増分PIDです。
P項、I項を結構意識していってください。
dはいらんかも

軽く俺の実装例を、example.cppに上げときます。

## 実際俺がやってる構成
```
位置PID
   ↓ 目標速度
速度PID
   ↓ 電流
モータ
```
というカスケード制御をやっています。





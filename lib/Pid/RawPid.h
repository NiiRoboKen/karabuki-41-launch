#pragma once

class PID {
    public:
        PID(double Kp, double Ki, double Kd) {
            this->Kp           = Kp;
            this->Ki           = Ki;
            this->Kd           = Kd;
            this->target       = 0;
            this->currentValue = 0;
            this->error        = 0;
            this->prevError    = 0;
            this->integral     = 0;
            this->derivative   = 0;
            this->output       = 0;
        }
        void setTarget(double target) { this->target = target; }
        // dt[s]
        double update(double currentValue, double dt) {
            this->error = this->target - currentValue;
            this->integral += this->error * dt;
            this->derivative = (this->error - this->prevError) / dt;
            this->output     = this->Kp * this->error + this->Ki * this->integral + this->Kd * this->derivative;
            this->prevError  = this->error;
            return output;
        }

    private:
        double Kp, Ki, Kd;   // ゲイン
        double target;       // 目標値
        double currentValue; // 実際の値
        double error;        // 偏差
        double prevError;    // 前回の偏差
        double integral;     // 積分項
        double derivative;   // 微分項
        double output;       // 出力
};

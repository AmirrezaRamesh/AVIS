#include "pid.h"

PID::PID(float kp, float ki, float kd, float constrain, float dt)
{
    this->k_p = kp;
    this->k_i = ki;
    this->k_d = kd;

    this->limit = constrain;

    this->error = 0;
    this->prev_error = 0;
    this->sum_error = 0;
    this->dt = dt;
}

void PID::set_error(float e)
{
    this->error = e;
}

float PID::set_constrain(const float &input)
{
    if (input > limit)
    {
        return limit;
    }
    if (input < -limit)
    {
        return -limit;
    }
    return input;
}

float PID::get_p()
{
    return k_p * error;
}

float PID::get_i()
{
    sum_error += k_i * error;
    return sum_error;
}

float PID::get_d()
{
    return k_d * (error - prev_error) / dt;
}

float PID::get_pid(float e)
{
    set_error(e);
    float pid_result = get_p() + get_i() + get_d();

    prev_error = error;

    return set_constrain(pid_result);
}
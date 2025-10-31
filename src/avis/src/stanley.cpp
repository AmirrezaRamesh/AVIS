#include "stanley.h"

float Stanley::radToDegree(const float &rad)
{
    return rad * 180 / M_PI;
}

float Stanley::degreeToRad(const float &degree)
{
    return degree * M_PI / 180;
}

Stanley::Stanley(const float &s, const float &d, const float &ks, const float &limit, const float &legal_error)
{
    this->gain_d = d;
    this->gain_steer = s;
    this->gain_ks = ks;

    prev_error = 0;

    this->limit = limit;
    this->legal_error = legal_error;
    error_buffers = deque<float>(8, 0.0f);
}

float Stanley::set_limits(const float &input)
{
    if (input > limit)
        return limit;
    if (input < -limit)
        return -limit;
    return input;
}

bool Stanley::is_valid_error(const float &error)
{
    return fabs(error - prev_error) < legal_error;
}

void Stanley::change_handler(float &error)
{
    if (!is_valid_error(error) && legal_error != -1)
    {
        float sum = 0;
        for (int i = 0; i < 8; i++)
        {
            sum += error_buffers[i];
        }
        error = sum / 8;
    }
    error_buffers.pop_front();
    error_buffers.push_back(error);
}

float Stanley::calculate_steer(const float &offset, float &heading_error, const float &velocity)
{
    float best_angle_error = heading_error;

    change_handler(best_angle_error);
    prev_error = best_angle_error;
    // degrees based on radian
    return set_limits((gain_steer * best_angle_error) + atan2(offset * gain_d, gain_ks + velocity));
}
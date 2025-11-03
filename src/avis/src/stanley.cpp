#include "stanley.h"

ofstream data_file("data.txt");

float Stanley::radToDegree(float rad)
{
    return rad * 180.0 / M_PI;
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

    this->limit = limit;
    this->legal_error = legal_error;
}

float Stanley::set_limits(float input)
{
    if (input > limit)
        return limit;
    if (input < -limit)
        return -limit;
    return input;
}
float Stanley::calculate_steer(float offset, float heading_error, float velocity)
{
    // degrees based on radian
    return set_limits((gain_steer * heading_error) + radToDegree(atan(offset * gain_d / (gain_ks + velocity))));
}

void Stanley::save_data(float angle, float offset, float curve, float speed, float steer)
{
    data_file << angle<<"        " << offset<<'\t' <<"                    "<<steer<<"      "<<speed<< endl;
}
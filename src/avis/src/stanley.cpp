#include <iostream>
#include <cmath>
#include "stanley.h"

using namespace std;
// private
float Stanley::radToDegree(const float &rad)
{
    return rad * 180 / pi;
}

float Stanley::degreeToRad(const float &degree)
{
    return degree * pi / 180;
}
// publics
Stanley::Stanley(const float &s, const float &d, const float &ks, const float &limit)
{
    this->gain_d = d;
    this->gain_steer = s;
    this->gain_ks = ks;

    pi = M_PI;

    this->limit = limit;
}

float Stanley::set_limits(const float &input)
{
    if (input > limit)
        return limit;
    if (input < -limit)
        return -limit;
    return input;
}

float Stanley::calculate_steer(const float &offset, const float &heading_error, const float &velocity)
{
    // degrees based on radian
    return set_limits((gain_steer*heading_error) + atan2(offset * gain_d, gain_ks + velocity));
}
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
Stanley::Stanley(const float &k, const float &ks, const float &limit)
{
    this->stanley_gain_k = k;
    this->stanley_gain_ks = ks;

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
    return set_limits(heading_error + atan2(offset * stanley_gain_k, stanley_gain_ks + velocity));
}
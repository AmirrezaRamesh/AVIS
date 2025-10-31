#ifndef LONGITUDINAL_CONTROLL
#define LONGITUDINAL_CONTROLL

#include <iostream>
#include "pid.cpp"
using namespace std;

class Longitudinal
{
private:
    PID speed_pid;

    float curve_area[5];
    float area_bound;
    float base_speed;
    float decreasment_speed;

public:
    Longitudinal(const float (&area)[5], float area_bound, float base_speed,float decreasment_speed);
    int determine_area(float carvature);
    float get_speed(float curvature);
    float set_speed(float curvature, float current_speed);
};

#endif
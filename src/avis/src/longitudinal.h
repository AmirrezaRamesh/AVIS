#ifndef LONGITUDINAL_CONTROLL
#define LONGITUDINAL_CONTROLL

#include <iostream>
#include <cmath>
#include "pid.h"
using namespace std;

class Longitudinal
{
private:
    PID speed_pid;

    float curve_area[10];
    float area_bound;
    float base_speed;
    float decreasment_speed;

    float k_angle;
    float k_offset;
    float k_curve;

public:
    Longitudinal(const int (&area)[10], float area_bound, float base_speed, float decreasment_speed, float (&pid_gains)[5],float k_angle,float k_offset,float k_curve);
    int determine_area(float carvature);
    float calculate_error(float angle,float offset,float curve);
    float get_speed(float curvature);
    float set_speed(float angle,float offset,float curvature, float current_speed);
};

#endif
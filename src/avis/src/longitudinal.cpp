#include "longitudinal.h"

// constructor
Longitudinal::Longitudinal(const int (&area)[10], float area_bound, float base_seed, float decreasment_speed, float (&pid_gains)[5],float k_angle,float k_offset,float k_curve)
    : speed_pid(pid_gains[0], pid_gains[1], pid_gains[2], pid_gains[3], pid_gains[4])
{
    this->base_speed = base_seed;
    this->area_bound = area_bound;
    this->decreasment_speed = decreasment_speed;

    this->k_angle=k_angle;
    this->k_offset=k_offset;
    this->k_curve=k_curve;

    for (int i = 0; i < 10; i++)
    {
        curve_area[i] = area[i];
    }
}

// -------------------------------------------------------------------------
int Longitudinal::determine_area(float curvature)
{
    for (int i = 0; i < 10; i++)
    {
        if (curvature > curve_area[i] - area_bound && curvature < curve_area[i] + area_bound)
        {
            return i + 1;
        }
    }
    return -1;
}

// -------------------------------------------------------------------------
float Longitudinal:: calculate_error(float angle,float offset,float curve){
    return k_angle*angle+k_offset*offset+k_curve*curve;
}

// -------------------------------------------------------------------------
float Longitudinal::get_speed(float curvature)
{
    int area = determine_area(curvature);
    return base_speed - area * decreasment_speed;
}

// -------------------------------------------------------------------------
float Longitudinal::set_speed(float angle,float offset,float curvature, float current_speed)
{
    // float reference_speed = get_speed(curvature);
    float error= calculate_error(fabs(angle),fabs(offset),fabs(curvature));
    return speed_pid.get_pid(150-error);
}
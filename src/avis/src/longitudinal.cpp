#include "longitudinal.h"

// constructor
Longitudinal::Longitudinal(const float (&area)[5], float area_bound, float base_seed, float decreasment_speed)
    : speed_pid(0.1, 0, 0, 5, 0.02)
{
    this->base_speed = base_seed;
    this->area_bound = area_bound;
    this->decreasment_speed = decreasment_speed;

    for (int i = 0; i < 5; i++)
    {
        curve_area[i] = area[i];
    }
}

// -------------------------------------------------------------------------
int Longitudinal::determine_area(float curvature)
{
    for (int i = 0; i < 5; i++)
    {
        if (curvature > curve_area[i] - area_bound && curvature < curve_area[i] + area_bound)
        {
            return i + 1;
        }
    }
    return -1;
}
// -------------------------------------------------------------------------
float Longitudinal::get_speed(float curvature)
{
    int area = determine_area(curvature);
    return base_speed - area * decreasment_speed;
}

float Longitudinal::set_speed(float curvature, float current_speed)
{
    float reference_speed = get_speed(curvature);
    return speed_pid.get_pid(reference_speed - current_speed);
}
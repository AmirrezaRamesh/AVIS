#include "longitudinal.h"

// constructor
Longitudinal::Longitudinal(const float (&area)[5], float area_bound, float base_seed)
{
    this->base_speed = base_seed;
    this->area_bound = area_bound;
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
    return base_speed / area;
}
#include "longitudinal.h"

// constructor
Longitudinal::Longitudinal(const float (&area)[5], float base_seed)

{
    this->base_speed = base_seed;
    for (int i = 0; i < 5; i++)
    {
        curve_area[i] = area[i];
    }
}

// -------------------------------------------------------------------------
int &Longitudinal::determine_area(float curvature)
{
    int area = -1;
    for (int i = 0; i < 5; i++)
    {
        if (curvature == curve_area[i])
        {
            area = i;
            return area;
        }
    }
    return area;
}
// -------------------------------------------------------------------------
float Longitudinal::get_speed(float curvature)
{
    int area = determine_area(curvature);
    return base_speed / area;
}
#ifndef LONGITUDINAL_CONTROLL
#define LONGITUDINAL_CONTROLL

#include <iostream>
using namespace std;

class Longitudinal
{
private:
    float curve_area[5];
    float area_bound;
    float base_speed;

public:
    Longitudinal(const float (&area)[5],float area_bound, float base_speed);
    int determine_area(float carvature);
    float get_speed(float curvature);
};

#endif
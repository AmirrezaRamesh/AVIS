#ifndef STANLEY
#define STANLEY

#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

class Stanley
{
private:
    float gain_d;
    float gain_steer;
    float gain_ks;

    float offset;
    float limit;

    float legal_error;
    

public:
    Stanley(const float &s, const float &d, const float &ks, const float &limit, const float &legal_error);

    float radToDegree(float rad);
    float degreeToRad(const float &degree);

    float set_limits(float input);
    void save_data(float angle, float ofsset, float curve, float speed, float steer);
    float calculate_error(float angle,float offset,float curve);

    float calculate_steer(float offset, float heading_error, float velocity);
};

#endif
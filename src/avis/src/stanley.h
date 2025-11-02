#ifndef STANLEY
#define STANLEY

#include <deque>
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
    float prev_error;
    float legal_error;
    deque<float> error_buffers;

public:
    Stanley(const float &s, const float &d, const float &ks, const float &limit, const float &legal_error);

    float radToDegree(const float &rad);
    float degreeToRad(const float &degree);

    float set_limits(const float &input);
    bool is_valid_error(const float &heading_error);
    void change_handler(float &error);
    void save_data(float angle, float ofsset, float curve, float speed, float steer);

    float calculate_steer(const float &offset, float &heading_error, const float &velocity);
};

#endif
#include <deque>
#ifndef STANLEY
#define STANLEY

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
    deque<float> error_buffers = {0, 0, 0, 0, 0, 0, 0, 0};

    float pi;

public:
    Stanley(const float &s, const float &d, const float &ks, const float &limit, const float &legal_error);

    float radToDegree(const float &rad);
    float degreeToRad(const float &degree);

    float set_limits(const float &input);
    bool is_valid_error(const float &heading_error);
    void change_handler(float &error);

    float calculate_steer(const float &offset, float &heading_error, const float &velocity);
};

#endif
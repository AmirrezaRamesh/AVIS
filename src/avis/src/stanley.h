#ifndef STANLEY
#define STANLEY

class Stanley
{
private:
    float gain_d;
    float gain_steer;
    float gain_ks;

    float offset;

    float pi;
    float limit;

public:
    Stanley(const float &s, const float &d, const float &ks, const float &limit);

    float radToDegree(const float &rad);
    float degreeToRad(const float &degree);

    float set_limits(const float &input);

    float calculate_steer(const float &offset, const float &heading_error, const float &velocity);
};

#endif
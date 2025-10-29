#ifndef STANLEY
#define STANLEY

class Stanley
{
private:
    float stanley_gain_k;
    float stanley_gain_ks;

    float offset;

    float pi;
    float limit;

public:
    Stanley(const float &k,const float &ks,const float &limit);

    float radToDegree(const float &rad);
    float degreeToRad(const float &degree);

    float set_limits(const float &input);

    float calculate_steer(const float &offset, const float &heading_error, const float &velocity);
};

#endif
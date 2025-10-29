#ifndef PID_controller
#define PID_controller

class PID
{
private:
    float k_p;
    float k_i;
    float k_d;

    float error;
    float prev_error;
    float sum_error;

    float limit;

    void set_error(float e);
    float set_constrain(const float &input);

public:
    PID(const float &kp, const float &ki, const float &kd, const float &constrains);
    float get_p();
    float get_d(float dt);
    float get_i();
    float get_pid(float e, float dt);
};

#endif
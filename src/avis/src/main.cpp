#include <iostream>
#include "longitudinal.cpp"

using namespace std;

int main()
{
    Longitudinal lng({1000, 2000, 3000, 4000, 5000}, 500, 50, 5);

    float current_speed = 30;

    float curvature;
    while (true)
    {
        cin >> curvature;
        float new_speed = lng.set_speed(curvature, current_speed);
        cout << "pid set: " << new_speed << endl;
        current_speed += new_speed;
        cout << "new current speed : " << current_speed << endl;
    }

    return 0;
}
#include <iostream>
#include "longitudinal.cpp"

using namespace std;

int main()
{
    Longitudinal lng({1000, 2000, 3000, 4000, 5000}, 500, 50);

    cout << lng.get_speed(434) << endl;
    cout << lng.determine_area(1860) << endl;

    return 0;
}
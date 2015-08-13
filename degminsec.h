#ifndef DEGMINSEC_H
#define DEGMINSEC_H

class DegMinSec
{
public:
    DegMinSec();
    DegMinSec(float degress, float minutes, float seconds);
    DegMinSec(float decimalDegress);

    float toDecimalDegress();

private:
    float degress;
    float minutes;
    float seconds;
};

#endif // DEGMINSEC_H

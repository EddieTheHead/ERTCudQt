#include "degminsec.h"
#include <math.h>

DegMinSec::DegMinSec()
{
    degress = 0;
    minutes = 0;
    seconds = 0;
}

DegMinSec::DegMinSec(float degress, float minutes, float seconds)
{
    this->degress = degress;
    this->minutes = minutes;
    this->seconds = seconds;
}

DegMinSec::DegMinSec(float decimalDegress)
{
    degress = floor(decimalDegress);
    minutes = floor(60 * (decimalDegress - degress));
    seconds = floor(3600 * (decimalDegress - degress - (float) minutes/60));
}

float DegMinSec::toDecimalDegress()
{
    return (float) degress + (float) minutes/60 + (float) seconds/3600;
}

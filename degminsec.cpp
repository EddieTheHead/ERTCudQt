#include "degminsec.h"
#include <math.h>

DegMinSec::DegMinSec()
{
    degress = 0;
    minutes = 0;
    seconds = 0;
}

DegMinSec::DegMinSec(int degress, int minutes, float seconds)
{
    this->degress = degress;
    this->minutes = minutes;
    this->seconds = seconds;
}

DegMinSec::DegMinSec(float decimalDegress)
{
    degress = floor(decimalDegress);
    minutes = floor(60 * (decimalDegress - degress));
    seconds = (3600 * (decimalDegress - degress - (float) minutes/60));
}

float DegMinSec::toDecimalDegress() const
{
    return (float) degress + (float) minutes/60 + (float) seconds/3600;
}

QString DegMinSec::toDMSString() const
{
    return QString("%1°%2'%3''").arg(degress).arg(minutes).arg(seconds,8);
}

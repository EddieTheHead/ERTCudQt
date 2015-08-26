#ifndef DEGMINSEC_H
#define DEGMINSEC_H

#include <QString>

class DegMinSec
{
public:
    DegMinSec();
    DegMinSec(int degress, int minutes, float seconds);
    DegMinSec(float decimalDegress);

    int deg() const
    {
        return degress;
    }
    int min() const
    {
        return minutes;
    }
    float sec() const
    {
        return seconds;
    }
    float toDecimalDegress() const;
    QString toDMSString() const;

private:
    int degress;
    int minutes;
    float seconds;
};

#endif // DEGMINSEC_H

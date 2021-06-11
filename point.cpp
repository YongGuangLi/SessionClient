#include "point.h"

Point::Point()
{

}

string Point::getName() const
{
    return name;
}

void Point::setName(const string &value)
{
    name = value;
}

string Point::getDir() const
{
    return dir;
}

void Point::setDir(const string &value)
{
    dir = value;
}

double Point::getValue() const
{
    return value;
}

char *Point::getPointeValue()
{
    return (char*)&value;
}

void Point::setValue(double value)
{
    this->value = value;
}

int64_t Point::getTime() const
{
    return time;
}

void Point::setTime(int64_t value)
{
    time = value;
}

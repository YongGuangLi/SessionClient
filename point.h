#ifndef POINT_H
#define POINT_H

#include <string>

using namespace std;

class Point
{
public:
    Point();

    string getName() const;
    void setName(const string &value);

    string getDir() const;
    void setDir(const string &value);

    double getValue() const;
    char *getPointeValue();
    void setValue(double value);

    int64_t getTime() const;
    void setTime(int64_t value);

private:
    string name; // required

    string dir; // required

    double value; // required

    int64_t time; // required
};

#endif // POINT_H

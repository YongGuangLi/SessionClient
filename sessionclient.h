#ifndef SESSIONCLIENT_H
#define SESSIONCLIENT_H

#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <memory> 
#include <vector>
#include "point.h"

using namespace std;

class Session;

class SessionClient
{
public:
    SessionClient(std::string host, int rpcPort, std::string username, std::string password);
    ~SessionClient();

    void putRealPoints(vector<Point> vecPoints);

    vector<Point> getRealPoints(vector<Point> vecPoints);
private:
    string createLastSql(vector<Point> vecPoints);
    Session *session;
};

#endif // SESSIONCLIENT_H

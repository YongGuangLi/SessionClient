#include "sessionclient.h"
#include "Session.h"

SessionClient::SessionClient(std::string host, int rpcPort, std::string username, std::string password)
{
    session = new Session(host, rpcPort, username, password);
    try
    {
        session->open(false);
    }
    catch (IoTDBConnectionException e)
    {
        std::cout<<e.what();
    }
}

SessionClient::~SessionClient()
{
    session->close();
    delete session;
    session = NULL;
}

void SessionClient::putRealPoints(vector<Point> vecPoints)
{
    map<string, vector<string>> mapDeviceIds;
    map<string, vector<vector<string>>> mapMeasurements;
    map<string, vector<vector<TSDataType::TSDataType>>> mapTypes;
    map<string, vector<vector<char*>>> mapValues;
    map<string, vector<int64_t>> mapTimestamps;

    for(int i = 0; i < vecPoints.size(); ++i)
    {
        Point point = vecPoints.at(i);
        string deviceId = point.getDir();
        string name = point.getName();
        int64_t time = point.getTime();
        string key = string().append(deviceId).append(".").append(to_string(point.getTime()));

        if(mapDeviceIds.count(key))
        {
            mapMeasurements[key].at(0).push_back(name);
            mapTypes[key].at(0).push_back(TSDataType::DOUBLE);
            mapValues[key].at(0).push_back(vecPoints.at(i).getPointeValue());
        }
        else
        {
            mapDeviceIds[key].push_back(deviceId);

            vector<string> vecMeasurements;
            vecMeasurements.push_back(name);
            mapMeasurements[key].push_back(vecMeasurements);

            vector<TSDataType::TSDataType> vecTypes;
            vecTypes.push_back(TSDataType::DOUBLE);
            mapTypes[key].push_back(vecTypes);

            vector<char*> vecValues;
            vecValues.push_back(vecPoints.at(i).getPointeValue());
            mapValues[key].push_back(vecValues);

            mapTimestamps[key].push_back(time);
        }
    }
    map<string, vector<string>>::iterator itDeviceIds;
    for(itDeviceIds = mapDeviceIds.begin(); itDeviceIds != mapDeviceIds.end(); ++itDeviceIds)
    {
        string key = itDeviceIds->first;
        session->insertRecords(mapDeviceIds[key], mapTimestamps[key], mapMeasurements[key], mapTypes[key], mapValues[key]);
    }
}

string SessionClient::createLastSql(vector<Point> vecPoints)
{
    string sql;
    for(int i = 0 ; i < vecPoints.size(); ++i)
    {
        Point point = vecPoints.at(i);
        if(i == 0)
            sql.append(point.getDir().substr(point.getDir().find(".") + 1) + "." + point.getName());
        else
            sql.append(" ," + point.getDir().substr(point.getDir().find(".") + 1) + "." + point.getName());
    }
    return "select last " + sql+ " from root";;
}

double ParseNumber(const char* s)
{
    bool bNegtiveBase,bNegtiveExp; 
    int nPreZero = 0; 
    const char* p;
    int sum_i = 0;
    double sum_f = 0.0;
    int sum_exp = 0;
    double sum = 0.0;
    bNegtiveBase  = false;
	bNegtiveExp = false;
    if(!s)
        return false;
    if('-' == *s)
    {
        bNegtiveBase = true;
        s++;
    }
    for(;'0' == *s;nPreZero++,s++); 
    for(;*s != '.' && *s != 'e' && *s != 'E' && *s != '\0';s++)
    {
        if(*s < '0' || *s > '9')
        {
            return false;
        }
        sum_i = sum_i*10 + *s - '0';
    }
    if(0 == sum_i && 0 == nPreZero) 
        return false;
    if('.' == *s) 
    {
        for(p = s;*p != 'e' && *p != 'E' && *p != '\0';p++); 
        if(s==p-1)
            return false;
        s = p;
        p--;
        for(;*p != '.';p--)
        {
            if(*p < '0' || *p > '9')
                return false;
            sum_f = sum_f*0.1 + 0.1*(*p - '0');
        }
    }
    if('e' == *s || 'E' == *s) 
    {
        s++;
        if('-' == *s)
        {
            bNegtiveExp = true;
            s++;
        }
        else if('+' == *s)
        {
            bNegtiveExp = false;
            s++;
        }
        nPreZero = 0;
        for(;*s != '\0';s++)
        {
            if(*s < '0' || *s > '9')
            {
                return false;
            }
            sum_exp = sum_exp*10 + *s - '0';
            nPreZero++;
        }
        if(0 == sum_exp && 0 == nPreZero)
            return false;
    }
    sum = sum_i + sum_f;
    if(bNegtiveExp) 
    {
        while(sum_exp > 0)
        {
            sum /= 10;
            sum_exp--;
        }
    }
    else
    {
        while(sum_exp > 0)
        {
            sum *= 10;
            sum_exp--;
        }
    }
    if(bNegtiveBase) 
        sum = -sum;

    return sum;
}

vector<Point> SessionClient::getRealPoints(vector<Point> vecPoints)
{
    map<string, Point> mapPoint;
    for(int i = 0; i < vecPoints.size(); ++i)
    {
        Point point = vecPoints.at(i);
        point.setTime(-28800L);
        mapPoint[point.getDir() + "." + point.getName()] = point;
    }

    string sql = createLastSql(vecPoints);

    unique_ptr<SessionDataSet> dataSet = session->executeQueryStatement(sql);
    dataSet->setBatchSize(1024);
    while (dataSet->hasNext())
    {
        string result = dataSet->next()->toString();
        int timeIndex = result.find("\t");
        string strTime = result.substr(0, timeIndex);

        int nameIndex = result.find("\t", timeIndex + 1);
        string strDir = result.substr(timeIndex + 1, nameIndex - timeIndex - 1);

        int valueIndex = result.find("\n");

        string strValue = result.substr(nameIndex + 1, valueIndex - nameIndex - 1);

        double value = ParseNumber(strValue.c_str());
		long long time = stoll(strTime);
        mapPoint[strDir].setTime(time);
        mapPoint[strDir].setValue(value);
    }

    dataSet->closeOperationHandle();

    vecPoints.clear();
    map<string, Point>::iterator itPoint;
    for(itPoint = mapPoint.begin(); itPoint != mapPoint.end(); ++itPoint)
        vecPoints.push_back(itPoint->second);

    return vecPoints;
}

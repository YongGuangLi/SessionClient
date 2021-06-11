#include "mainwindow.h"
#include "ui_mainwindow.h"

int64_t TimeMilliSecond()
{
#ifdef WIN32
    timeb now;
    ftime(&now);
    return now.time * 1000 + now.millitm;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + (long long)tv.tv_usec / 1000;
#endif
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SessionClient *sessionClient = new SessionClient("111.22.2.188", 7912, "test", "123456");

    vector<Point> points;
    Point point1;
    for(int i = 0; i <= 300; ++i)
    {
        point1.setDir("root.DTXY.TEST");
        point1.setName("TAG00000001");
        point1.setTime(TimeMilliSecond() + i);
        point1.setValue(i);
        points.push_back(point1);
    }
    sessionClient->putRealPoints(points);

    Point point;
    point.setDir("root.DTXY.TEST");
    point.setName("TAG00000001");
    points.push_back(point);
    vector<Point> nps = sessionClient->getRealPoints(points);
    Point p = nps[0];
    qDebug()<<p.getValue();
}

MainWindow::~MainWindow()
{
    delete ui;
}

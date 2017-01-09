#ifndef TIMELOGGER_H
#define TIMELOGGER_H

#include <QObject>
#include <MCTypes>
#include <vector>

class TimeLogger : public QObject
{
    Q_OBJECT

public:
    TimeLogger();

    ~TimeLogger();

    void save();

    void setTrack(QString rata);

public slots:
    void addLap(MCUint index, int msec);

private:
    QString log;
    int lap[30];
    int sum[30];

};

#endif // TIMELOGGER_H

#include "timelogger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <vector>
#include <algorithm>

TimeLogger::TimeLogger()
{
    for(int i=0;i<30;i++){
        lap[i] = 0;
        sum[i] = 0;
    }
    this->log=QDateTime::currentDateTime().toString()+"\n";
}

TimeLogger::~TimeLogger()
{
    save();
}

const int LAPTOCOUNT = 4;

void TimeLogger::addLap(MCUint index, int msec)
{

    lap[index]++;

    if(lap[index]<=LAPTOCOUNT){

        sum[index]+=msec;

        if(lap[index]==LAPTOCOUNT)
            //this->log.append(QString::number(index)+" "+QString::number(lap[index])+" "+QString::number(msec)+"\n");
            this->log.append(QString::number(index)+" "+QString::number(lap[index])+" "+QString::number(sum[index])+"\n");
    }
}

int median(std::vector<int> &v)
{
    size_t n = v.size() / 2;
    nth_element(v.begin(), v.begin()+n, v.end());
    return v[n];
}

void TimeLogger::save()
{
    //lasketaan erojen mediaani
    std::vector<int> erot;
    for(int i=11;i>2;i-=2){
        erot.push_back(sum[i-1]-sum[i]);
        this->log.append(" " + QString::number(sum[i-1]-sum[i]) + "\n");
    }
    int mediaani = median(erot);
    this->log.append("erojen mediaani: " + QString::number(mediaani) + "\n");


    QString filename="laptimes.log";
    QFile file( filename );
    if ( file.open(QIODevice::Append) )
    {
        QTextStream stream( &file );
        stream << log << endl;
    }
}

void TimeLogger::setTrack(QString rata)
{
    this->log.append(rata+"\n");
}

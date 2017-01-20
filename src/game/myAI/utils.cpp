#include "utils.h"
#include <QTime>

double calcAngle(QPointF p1, QPointF p2)
{
 double a_x = p2.x() - p1.x();
 double a_y = p2.y() - p1.y();

 double b_x = 1.0;
 double b_y = 0.0;

 return acos((a_x*b_x+a_y*b_y)/sqrt(a_x*a_x+a_y*a_y));
}

double calcDistance(QPointF p1, QPointF p2){
    QPointF ero = p1 - p2;
    return sqrt((ero.x()*ero.x())+(ero.y()*ero.y()));
}

void seedRand(){
    // Create seed for the random
    // That is needed only once on application startup
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
}

int randInt(int low, int high){
    return qrand() % ((high + 1) - low) + low;
}

double nelio(double luku){
    return luku*luku;
}

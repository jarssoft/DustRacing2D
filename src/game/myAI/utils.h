#ifndef UTILS_H
#define UTILS_H

#include <QPointF>
#include <math.h>

const double PI = 3.141592;

//laskee kahden pisteen välisen kulman
double calcAngle(QPointF p1, QPointF p2);

//laskee kahden pisteen välisen etäisyyden
double calcDistance(QPointF p1, QPointF p2);

void seedRand();

//random number between low and high
int randInt(int low, int high);

//palauttaa syötteen neliön
double nelio(double);

#endif // UTILS_H

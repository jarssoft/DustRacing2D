#include <assert.h>
#include "kolmio.h"
#include "math.h"
#include "utils.h"

Kolmio::Kolmio(){
    abl=UNSET;
    bcl=UNSET;
    cal=UNSET;
    aa=UNSET;
    ba=UNSET;
    ca=UNSET;
}

void Kolmio::setABLenght(double lenght){
    abl = lenght;
}

void Kolmio::setBCLenght(double lenght){
    //assert(aa == UNSET);
    //assert(ca == UNSET);
    //assert(cal == UNSET);
    bcl = lenght;
}

void Kolmio::setCALenght(double lenght){
    cal = lenght;
}

void Kolmio::setAAngle(double rad){
    aa = rad;
}

void Kolmio::setBAngle(double rad){
    ba = rad;
}

void Kolmio::setCAngle(double rad){
    ca = rad;
}

double Kolmio::getCALenght(){
    return getLenght(cal, abl, bcl, ba);
}

double Kolmio::getABLenght(){
    return getLenght(abl, bcl, cal, ca);
}

double Kolmio::getBCLenght(){
    return getLenght(bcl, cal, abl, aa);
}

double Kolmio::getLenght(double& pituus, double& vasensivu, double& oikeasivu, double& kulma){

    if(pituus==UNSET)
        if(vasensivu!=UNSET && oikeasivu!=UNSET && kulma!=UNSET)
            pituus = sqrt(nelio(vasensivu)+nelio(oikeasivu) - 2 * vasensivu * oikeasivu * cos(kulma));

    return pituus;
}

double Kolmio::suhde(){

    if(bcl!=UNSET && aa!=UNSET)
        kolmionSuhde = bcl / sin(aa);
    if(cal!=UNSET && ba!=UNSET)
        kolmionSuhde = cal / sin(ba);
    if(abl!=UNSET && ca!=UNSET)
        kolmionSuhde = abl / sin(ca);

    return kolmionSuhde;
}

double Kolmio::getAAngle(){
    //return getAngle(aa, bcl);
    return getAngle(aa, abl, cal, bcl);
}

double Kolmio::getBAngle(){
    //return getAngle(ba, cal);
    return getAngle(ba, bcl, abl, cal);
}

double Kolmio::getCAngle(){
    //return getAngle(ca, abl);
    return getAngle(ca, cal, bcl, abl);
}

double Kolmio::getAngle(double& kulma, double& vastakkainenSivu){

    //http://www.cprogramto.com/given-the-sides-of-a-triangle-find-angle-in-c/

    if(kulma==UNSET)
        if(vastakkainenSivu > kolmionSuhde)
            kulma = asin(vastakkainenSivu / kolmionSuhde);

    return kulma;
}

double Kolmio::getAngle(double& kulma, double& vasensivu, double& oikeasivu, double& vastakkainensivu){
    if(kulma==UNSET){
        assert(vasensivu!=UNSET && oikeasivu!=UNSET && vastakkainensivu!=UNSET);
        kulma = acos((nelio(vasensivu)+nelio(oikeasivu)-nelio(vastakkainensivu)) / (2 * vasensivu * oikeasivu));
    }

    return kulma;
}



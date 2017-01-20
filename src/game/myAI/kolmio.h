#ifndef KOLMIO_H
#define KOLMIO_H

const int UNSET = -1000;

//ratkaisee kolmion suhteita (sivun pituuksia ja kulmia) trigonometrian avulla
//http://en.wikipedia.org/wiki/Solution_of_triangles
class Kolmio{

public:
    Kolmio();

    //asettaa sivujen pituudet
    void setABLenght(double rad);
    void setBCLenght(double rad);
    void setCALenght(double rad);

    //asettaa kulmat
    void setAAngle(double rad);
    void setBAngle(double rad);
    void setCAngle(double rad);

    //ratkaisee sivujen pituudet
    double getABLenght();
    double getBCLenght();
    double getCALenght();

    double suhde();

    //ratkaisee kulmat
    double getAAngle();
    double getBAngle();
    double getCAngle();

private:

    //pituudet
    double abl, bcl, cal;

    //kulmat
    double aa, ba, ca;

    //palauttaa halutun sivun pituuden
    double getLenght(double& pituus, double& vasensivu, double& oikeasivu, double& kulma);

    //laskee halutun kulman
    double getAngle(double& kulma, double& vastakkainenSivu);

    //laskee halutun kulman
    double getAngle(double& kulma, double& vasensivu, double& oikeasivu, double& vastakkainensivu);

    //muistissa säilytettävä, kaikille kulmille yhteinen välimuuttuja
    //englanniksi "circumcircle"
    double kolmionSuhde;

};

#endif // KOLMIO_H


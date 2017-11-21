#pragma once
#ifndef FACETTE_H
#define FACETTE_H
#include "point3.h"

class Facette
{
private:

	point3 A;
	point3 B;
	point3 C;

public:

	point3 getA();
	void setA(point3 A);
	point3 getB();
	void setB(point3 B);
	point3 getC();
	void setC(point3 C);
	Facette(point3 A, point3 B, point3 C);

};
#endif
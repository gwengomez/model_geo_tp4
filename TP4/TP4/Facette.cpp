#include "Facette.h"

point3 Facette::getA()
{
	return A;
}

void Facette::setA(point3 A)
{
	this->A.x = A.x;
	this->A.y = A.y;
	this->A.z = A.z;
}

point3 Facette::getB()
{
	return B;
}

void Facette::setB(point3 B)
{
	this->B.x = B.x;
	this->B.y = B.y;
	this->B.z = B.z;
}

point3 Facette::getC()
{
	return C;
}

void Facette::setC(point3 C)
{
	this->C.x = C.x;
	this->C.y = C.y;
	this->C.z = C.z;
}

Facette::Facette(point3 A, point3 B, point3 C)
{
	setA(A);
	setB(B);
	setC(C);
}
#include "MyVector.h"

#include <iostream>	

using namespace std;


Vector::Vector (double c1, double c2)
{
	x=c1;   y=c2;
}

Vector::Vector ()
{
	x = y = 0.;
}

void Vector::Out()
{
	cout << "\nVector:  x = " << x << ",  y = " << y;
}

void Vector::print() {
	cout << "x: " << x << " y: " << y << endl;
}

//====== Переопределение операций =====//
Vector& Vector::operator= (const Vector& v)	// Присвоение
{
	if (this == &v)
		return *this;
	x = v.x;
	y = v.y;
	return *this;
}

bool Vector::operator<(const Vector& v) const {
    double first_module_sq = this->getX() * this->getX() + this->getY() * this->getY();
    double second_module_sq = v.getX() * v.getX() + v.getY() * v.getY();
    
    return first_module_sq < second_module_sq;
}

bool Vector::operator==(const Vector& v) const {
	if (this->getX() == v.getX() && this->getY() == v.getY()) {
		return true;
	}
	return false;
}

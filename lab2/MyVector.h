// Класс Vector, инкапсулируюет функциональность вектора на плоскости
// Добавьте в класс все необходимые конструкторы и методы необходимые для функционирования в данной лабораторной
#include <ostream>

class Vector
{
private:
	double x, y;	// Координаты вектора на плоскости
public:
	//========== Три конструктора
	Vector (double c1, double c2);
	Vector ();							// Default

	// template <typename T>
	// void Swap(T& first_value, T& second_value) {
	// 	T temporary_variable = first_value;
	// 	first_value = second_value;
	// 	second_value = temporary_variable;
	// }
	friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
        os << "(" << v.getX() << ", " << v.getY() << ")"; 
        return os;
    }
	
	double getX() const { return x; };
	double getY() const { return y; };
	void print();
	//====== Переопределение операций =====//
	Vector& operator= (const Vector& v);	// Присвоение
	bool operator<(const Vector& v) const;
	bool operator==(const Vector& v) const;
	void Out();
};

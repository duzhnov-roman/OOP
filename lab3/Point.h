#pragma once

//#include "Point.cpp"

class Point {
        double x, y;
    public:
        double getX() const { return x; }
        double getY() const { return y; }

        double setX(double x) { this->x = x; }
        double setY(double y) { this->y = y; }

        friend std::ostream& operator<<(std::ostream& os, const Point& point);

        Point(double firstValue, double secondValue);
};

std::ostream& operator<<(std::ostream& os, const Point& point) {
    os << "Point: (" << point.getX() << ", " << point.getY() << ") "; 

    return os;
}
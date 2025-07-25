#ifndef RDP_GEOMETRY_H
#define RDP_GEOMETRY_H

#include <array>
#include <cmath>
#include <algorithm>
#include <initializer_list>

namespace rdp {

struct Vec2D {
    double x = 0.0;
    double y = 0.0;

    Vec2D() = default;
    Vec2D(double x_, double y_) : x(x_), y(y_) {}

    Vec2D operator+(const Vec2D& other) const {
        return Vec2D(x + other.x, y + other.y);
    }

    Vec2D operator-(const Vec2D& other) const {
        return Vec2D(x - other.x, y - other.y);
    }

    Vec2D operator*(double s) const {
        return Vec2D(x * s, y * s);
    }

    double dot(const Vec2D& other) const {
        return x * other.x + y * other.y;
    }

    double lengthSquared() const {
        return dot(*this);
    }

    double length() const {
        return std::sqrt(lengthSquared());
    }
};

struct Point2D {
    double x = 0.0;
    double y = 0.0;

    Point2D() = default;
    Point2D(double x_, double y_) : x(x_), y(y_) {}

    Vec2D operator-(const Point2D& other) const {
        return Vec2D(x - other.x, y - other.y);
    }

    Point2D operator+(const Vec2D& v) const {
        return Point2D(x + v.x, y + v.y);
    }
};

inline double point2LineDistanceSquared2D(const Point2D &p, const Point2D &a, const Point2D &b) {
    Vec2D ab = b - a;

    if (ab.lengthSquared() < 1.0e-14)
        return (p - a).lengthSquared();

    // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
    double distance = ab.y * (p.x - a.x) - ab.x * (p.y - a.y);
    
    return (distance * distance) / ab.lengthSquared();
}

// For 3D:
// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Another_vector_formulation
// return cross(p - a, ab).lengthSquared() / ab.lengthSquared();

} // namespace rdp
#endif // RDP_GEOMETRY_H
#ifndef RDP_GEOMETRY_H
#define RDP_GEOMETRY_H

#include <array>
#include <cmath>
#include <algorithm>
#include <initializer_list>

namespace rdp {

template <std::size_t N>
struct Vec; // no definition here

template <>
struct Vec<2> {
    double x = 0.0;
    double y = 0.0;

    Vec<2>() = default;
    Vec<2>(double x_, double y_) : x(x_), y(y_) {}

    // old
    Vec<2> operator+(const Vec<2>& other) const {
        return Vec<2>(x + other.x, y + other.y);
    }

    Vec<2> operator-(const Vec<2>& other) const {
        return Vec<2>(x - other.x, y - other.y);
    }

    Vec<2> operator*(double s) const {
        return Vec<2>(x * s, y * s);
    }

    double dot(const Vec<2>& other) const {
        return x * other.x + y * other.y;
    }

    // 2D-cross product
    double cross(const Vec& other) const {
        return x * other.y - y * other.x;
    }

    double lengthSquared() const {
        return dot(*this);
    }

    double length() const {
        return std::sqrt(lengthSquared());
    }
};

// Empty definition
template <std::size_t N>
struct Point;

template <>
struct Point<2> {
    double x = 0.0;
    double y = 0.0;

    Point() = default;
    Point(double x_, double y_) : x(x_), y(y_) {}

    double& operator[](std::size_t i) {
        return i == 0 ? x : y;
    }

    const double& operator[](std::size_t i) const {
        return i == 0 ? x : y;
    }
    
    Vec<2> operator-(const Point<2> &other) const {
        return Vec<2>{x - other.x, y - other.y};
    }

    Point<2> operator+(const Vec<2> &v) const {
        return Point<2>{x + v.x, y + v.y};
    }
};

template <std::size_t N>
inline double point2LineDistanceSquared(const Point<N>& p, const Point<N>& a, const Point<N>& b) {
    static_assert(N == 2, "Only 2D is supported.");

    Vec<N> ab = b - a;

    if (ab.lengthSquared() < 1.0e-14)
        return (p - a).lengthSquared();

    // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
    Vec<N> ap = p - a;
    double crossVal = (ab).cross(ap);

    return (crossVal * crossVal) / ab.lengthSquared();
}

} // namespace rdp
#endif // RDP_GEOMETRY_H
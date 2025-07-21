#ifndef RDP_GEOMETRY_H
#define RDP_GEOMETRY_H

#include <array>
#include <cmath>
#include <algorithm>
#include <initializer_list>

namespace rdp {

template <std::size_t N>
struct Vec {
    std::array<double, N> data{};

    Vec() { data.fill(0); }
    Vec(std::initializer_list<double> vals) {
        std::copy(vals.begin(), vals.end(), data.begin());
    }

    Vec operator+(const Vec &other) const {
        Vec result;
        for (std::size_t i = 0; i < N; ++i)
            result.data[i] = data[i] + other.data[i];
        return result;
    }

    Vec operator-(const Vec &other) const {
        Vec result;
        for (std::size_t i = 0; i < N; ++i)
            result.data[i] = data[i] - other.data[i];
        return result;
    }

    Vec operator*(double s) const {
        Vec result;
        for (std::size_t i = 0; i < N; ++i)
            result.data[i] = data[i] * s;
        return result;
    }

    double dot(const Vec &other) const {
        double sum = 0;
        for (std::size_t i = 0; i < N; ++i)
            sum += data[i] * other.data[i];
        return sum;
    }

    double lengthSquared() const { return dot(*this); }
    double length() const { return std::sqrt(lengthSquared()); }
};

template <std::size_t N>
struct Point {
    std::array<double, N> data{};

    Point() { data.fill(0); }
    Point(std::initializer_list<double> vals) {
        std::copy(vals.begin(), vals.end(), data.begin());
    }

    Vec<N> operator-(const Point &other) const {
        Vec<N> result;
        for (std::size_t i = 0; i < N; ++i)
            result.data[i] = data[i] - other.data[i];
        return result;
    }

    Point operator+(const Vec<N> &v) const {
        Point result;
        for (std::size_t i = 0; i < N; ++i)
            result.data[i] = data[i] + v.data[i];
        return result;
    }
};

template <std::size_t N>
double point2LineDistanceSquared(const Point<N> &p, const Point<N> &a, const Point<N> &b) {
    Vec<N> ab = b - a;

    if (ab.lengthSquared() < 1.0e-14)
        return (p - a).lengthSquared();

    if constexpr (N == 2) {
        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
        double distance = ab.data[1] * (p.data[0] - a.data[0]) - ab.data[0] * (p.data[1] - a.data[1]);
        return (distance * distance) / ab.lengthSquared();
    } else {
        static_assert(N == 2, "Only 2D is supported.");
    }
}

} // namespace rdp
#endif // RDP_GEOMETRY_H
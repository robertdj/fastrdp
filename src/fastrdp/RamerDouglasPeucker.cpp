#include <cstddef>
#include <cassert>
#include <tuple>
#include <vector>
#include <array>

namespace rdp {
struct Vec3D
{
    double x, y, z;

    Vec3D() : x(0), y(0), z(0) {}
    Vec3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    Vec3D operator+(const Vec3D &other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vec3D operator-(const Vec3D &other) const { return {x - other.x, y - other.y, z - other.z}; }
    Vec3D operator*(double s) const { return {x * s, y * s, z * s}; }

    double dot(const Vec3D &other) const { return x * other.x + y * other.y + z * other.z; }
    Vec3D cross(const Vec3D &other) const {
        return {y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x};
    }

    double length() const { return std::sqrt(x * x + y * y + z * z); }
    double lengthSquared() const { return x * x + y * y + z * z; }
};

template <std::size_t N>
struct Vec {
    std::array<double, N> data{};

    Vec() { data.fill(0); }
    Vec(std::initializer_list<double> vals) {
        std::copy(vals.begin(), vals.end(), data.begin());
    }

    Vec operator+(const Vec &other) const {
        Vec result;
        for (std::size_t i = 0; i < N; ++i) result.data[i] = data[i] + other.data[i];
        return result;
    }

    Vec operator-(const Vec &other) const {
        Vec result;
        for (std::size_t i = 0; i < N; ++i) result.data[i] = data[i] - other.data[i];
        return result;
    }

    Vec operator*(double s) const {
        Vec result;
        for (std::size_t i = 0; i < N; ++i) result.data[i] = data[i] * s;
        return result;
    }

    double dot(const Vec &other) const {
        double sum = 0;
        for (std::size_t i = 0; i < N; ++i) sum += data[i] * other.data[i];
        return sum;
    }

    double lengthSquared() const { return dot(*this); }
    double length() const { return std::sqrt(lengthSquared()); }
};

inline Vec<3> cross(const Vec<3> &a, const Vec<3> &b) {
    return {
        a.data[1] * b.data[2] - a.data[2] * b.data[1],
        a.data[2] * b.data[0] - a.data[0] * b.data[2],
        a.data[0] * b.data[1] - a.data[1] * b.data[0]
    };
}

struct Point3D
{
    double x, y, z;

    Point3D() : x(0), y(0), z(0) {}
    Point3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    Vec3D operator-(const Point3D &other) const { return Vec3D{x - other.x, y - other.y, z - other.z}; }
    Point3D operator+(const Vec3D &v) const { return Point3D{x + v.x, y + v.y, z + v.z}; }
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
        for (std::size_t i = 0; i < N; ++i) result.data[i] = data[i] - other.data[i];
        return result;
    }
    Point operator+(const Vec<N> &v) const {
        Point result;
        for (std::size_t i = 0; i < N; ++i) result.data[i] = data[i] + v.data[i];
        return result;
    }
};

template <std::size_t N>
double point2LineDistanceSquared(const Point<N> &p, const Point<N> &a, const Point<N> &b) {
    // Compute the squared distance between p and the line defined by b - a

    // Create the line
    Vec<N> ab = b - a;

    // If a == b --> dist = (p - a)**2
    if (ab.lengthSquared() < 1.0e-14)
        return (p - a).lengthSquared();

    if constexpr (N == 2) { // 2D
        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
        // Some linear algebra
        // The point
        // x0 = p.data[0]
        // y0 = p.data[1]
        // a
        // x1 = a.data[0]
        // y1 = a.data[1]
        // b
        // x2 = b.data[0]
        // y2 = b.data[1]
        // The line
        // x2 - x1 = ab.data[0]
        // y2 - y1 = ab.data[1]
        // Therefore,
        // ((y2 - y1) * x0 + (x2 - x1) * y0 + x2 * y1 - y2 * x1)
        // can be written as
        // (ab.data[1] * p.data[0] + ab.data[0] * p.data[1] + b.data[0] * a.data[1] - b.data[1] * a.data[0])
        // and
        // sqrt((y2 - y1)**2 + (x2 - x1)**2)**2
        // can be written as
        // ab.lengthSquared()

        // Calculate the numerator
        distance = (ab.data[1] * p.data[0] + ab.data[0] * p.data[1] + b.data[0] * a.data[1] - b.data[1] * a.data[0])

        // Return the square of the distance
        return (distance * distance) / ab.lengthSquared();
    } else if constexpr (N == 3) { // 3D
        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Another_vector_formulation
        return cross(p - a, ab).lengthSquared() / ab.lengthSquared();
    } else {
        static_assert(N == 2 || N == 3, "Only 2D and 3D supported.");
    }
}

// Find the point furthest away from reference (points[startIndex] == points[endIndex])
template <std::size_t N>
std::pair<double, std::size_t> findMostDistantPoint(const std::vector<Point<N>> &points,
                                                    std::size_t startIndex, std::size_t endIndex)
{
    assert(startIndex < endIndex && "Start index must be smaller than end index");
    assert(endIndex < points.size() && "End index is larger than the number of points");
    assert(points.size() >= 2 && "At least two points needed");

    assert((points[startIndex] - points[endIndex]).lengthSquared() == 0 && "Start and end point must be equal");

    double maxDistanceSquared = 0.0;
    std::size_t maxDistanceIndex = startIndex;

    for (std::size_t i = startIndex + 1; i != endIndex; ++i)
    {
        double distanceSquared = (points[i] - points[startIndex]).lengthSquared();

        if (distanceSquared > maxDistanceSquared)
        {
            maxDistanceIndex = i;
            maxDistanceSquared = distanceSquared;
        }
    }

    return std::make_pair(maxDistanceSquared, maxDistanceIndex);
}


// Find the point with the maximum distance from line between start and end.
// Rearranging this formula to avoid recomputing constants:
// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
template <std::size_t N>
std::pair<double, std::size_t> findMostDistantPointFromLine(const std::vector<Point<N>> &points,
                                                            std::size_t startIndex,
                                                            std::size_t endIndex)
{
    assert(startIndex < endIndex && "Start index must be smaller than end index");
    assert(endIndex < points.size() && "End index is larger than the number of points");
    assert(points.size() >= 2 && "At least two points needed");

    Vec<N> lineDiff = points[endIndex] - points[startIndex];
    double lineLengthSquared = lineDiff.lengthSquared();

    if (lineLengthSquared == 0)
    {
        return findMostDistantPoint(points, startIndex, endIndex);
    }

    // double offset = points[startIndex].y * lineDiff.x - points[startIndex].x * lineDiff.y; // 2D

    double maxDistanceSquared = 0.0;
    std::size_t maxDistanceIndex = startIndex;

    for (std::size_t i = startIndex + 1; i != endIndex; ++i)
    {
        // double unscaledDistance = offset - points[i].y * lineDiff.x + points[i].x * lineDiff.y; // 2D
        // Vec<N> diff = points[i] - points[startIndex];
        // double unscaledDistanceSquared = diff.cross(lineDiff).lengthSquared();
        double distanceSquared = point2LineDistanceSquared(points[i], points[startIndex],  points[endIndex]
        ) 
        if (distanceSquared > maxDistanceSquared)
        {
            maxDistanceIndex = i;
            maxDistanceSquared = distanceSquared;
        }
    }

    // maxDistanceSquared /= lineLengthSquared;

    // Constructor is faster than initialization
    return std::make_pair(maxDistanceSquared, maxDistanceIndex);
}

template <std::size_t N>
void RamerDouglasPeucker(const std::vector<Point<N>> &points, std::size_t startIndex,
                         std::size_t endIndex, double epsilonSquared,
                         std::vector<std::size_t> &indicesToKeep)
{
    assert(startIndex < endIndex && "Start index must be smaller than end index");
    assert(endIndex < points.size() && "End index is larger than the number of points");
    // The inequalities 0 <= startIndex < endIndex < points.size() imply that points.size() >= 2
    assert(points.size() >= 2 && "At least two points needed");

    assert(epsilonSquared >= 0 && "epsilonSquared must be non-negative");

    assert(indicesToKeep.size() >= 1 && "indicesToKeep should be non-empty");
    assert(indicesToKeep[0] == 0 && "indicesToKeep should be initialized with a 0");

    auto [maxDistanceSquared, maxDistanceIndex] =
        findMostDistantPointFromLine(points, startIndex, endIndex);

    if (maxDistanceSquared > epsilonSquared)
    {
        RamerDouglasPeucker(points, startIndex, maxDistanceIndex, epsilonSquared, indicesToKeep);
        RamerDouglasPeucker(points, maxDistanceIndex, endIndex, epsilonSquared, indicesToKeep);
    }
    else
    {
        // startIndex is included from the previous run because we execute sequentially with the
        // lower parts of the list first
        indicesToKeep.push_back(endIndex);
    }
}
} // end namespace rdp

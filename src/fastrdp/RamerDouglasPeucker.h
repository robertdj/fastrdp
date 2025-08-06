#include <cassert>
#include <tuple>
#include <vector>
#include <array>
#include <cmath>
#include <cstddef>
#include <type_traits>

namespace rdp
{
    template <std::size_t N>
    struct Point {
        std::array<double, N> coords{};

        Point() = default;

        Point(const std::array<double, N>& coords) : coords(coords) {}

        Point(double x, double y) {
            static_assert(N == 2, "This constructor is only valid for N=2");
            coords[0] = x;
            coords[1] = y;
        }

        bool operator==(const Point& other) const {
            return coords == other.coords;
        }
    };

    template <std::size_t N>
    struct Vector {
        std::array<double, N> coords{};

        Vector() = default;

        Vector(const Point<N>& p1, const Point<N>& p2) {
            for (std::size_t i = 0; i < N; ++i) {
                coords[i] = p2.coords[i] - p1.coords[i];
            }
        }

        double abs2() const {
            double sum = 0.0;
            for (const auto& coord : coords) {
                sum += coord * coord;
            }
            return sum;
        }

        double dot(const Vector& other) const {
            double result = 0.0;
            for (std::size_t i = 0; i < N; ++i) {
                result += coords[i] * other.coords[i];
            }
            return result;
        }

        Vector normalized2() const {
            double length = abs2();
            Vector result;

            if (length == 0) {
                return result;
            }

            for (std::size_t i = 0; i < N; ++i) {
                result.coords[i] = coords[i] / length;
            }
            return result;
        }

        Vector scale(double factor) const {
            Vector result;
            for (std::size_t i = 0; i < N; ++i) {
                result.coords[i] = coords[i] * factor;
            }
            return result;
        }

        Vector operator-(const Vector& other) const {
            Vector result;
            for (std::size_t i = 0; i < N; ++i) {
                result.coords[i] = coords[i] - other.coords[i];
            }
            return result;
        }
    };

    template <std::size_t N>
    struct Subspace {
        Vector<N> basis;
        Vector<N> direction;

        Subspace(const Point<N>& p1, const Point<N>& p2)
        {
            basis = Vector<N>(p1, p2);
            direction = basis.normalized2();
        }

        bool isNull() const {
            return basis.abs2() == 0;
        }

        Vector<N> project(const Vector<N>& w) const {
            double dotProduct = w.dot(direction);
            return basis.scale(dotProduct);
        }

        double distance2(const Vector<N>& w) const {
            auto projected = project(w);
            return (w - projected).abs2();
        }
    };

    // Find the point furthest away from reference(points[startIndex] == points[endIndex])
    template <std::size_t N>
    std::pair<double, std::size_t> findMostDistantPoint(const std::vector<Point<N>>& points,
        std::size_t startIndex, std::size_t endIndex)
    {
        assert(startIndex < endIndex && "Start index must be smaller than end index");
        assert(endIndex < points.size() && "End index is larger than the number of points");
        assert(points.size() >= 2 && "At least two points needed");

        assert(points[startIndex] == points[endIndex] && "Start and end point must be equal");

        double maxDistanceSquared = 0.0;
        std::size_t maxDistanceIndex = startIndex;

        for (std::size_t i = startIndex + 1; i != endIndex; ++i)
        {
            auto v = Vector<N>(points[startIndex], points[i]);
            double distanceSquared = v.abs2();

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
    std::pair<double, std::size_t> findMostDistantPointFromLine(const std::vector<Point<N>>& points,
        std::size_t startIndex,
        std::size_t endIndex)
    {
        assert(startIndex < endIndex && "Start index must be smaller than end index");
        assert(endIndex < points.size() && "End index is larger than the number of points");
        assert(points.size() >= 2 && "At least two points needed");

        auto lineDiff = Subspace(points[startIndex], points[endIndex]);

        if (lineDiff.isNull())
        {
            return findMostDistantPoint(points, startIndex, endIndex);
        }

        double maxDistanceSquared = 0.0;
        std::size_t maxDistanceIndex = startIndex;

        for (std::size_t i = startIndex + 1; i != endIndex; ++i)
        {
            auto v = Vector<N>(points[startIndex], points[i]);
            double distanceSquared = lineDiff.distance2(v);

            if (distanceSquared > maxDistanceSquared)
            {
                maxDistanceIndex = i;
                maxDistanceSquared = distanceSquared;
            }
        }

        // Constructor is faster than initialization
        return std::make_pair(maxDistanceSquared, maxDistanceIndex);
    }

    template <std::size_t N>
    void RamerDouglasPeucker(const std::vector<Point<N>>& points, std::size_t startIndex,
        std::size_t endIndex, double epsilonSquared,
        std::vector<std::size_t>& indicesToKeep)
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

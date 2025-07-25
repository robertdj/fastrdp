#include <cstddef>
#include <cassert>
#include <tuple>
#include <vector>
#include <array>
#include "Geometry.h"

namespace rdp {

// Find the point furthest away from reference (points[startIndex] == points[endIndex])
std::pair<double, std::size_t> findMostDistantPoint2D(const std::vector<Point2D> &points,
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
std::pair<double, std::size_t> findMostDistantPointFromLine2D(const std::vector<Point2D> &points,
                                                            std::size_t startIndex,
                                                            std::size_t endIndex)
{
    assert(startIndex < endIndex && "Start index must be smaller than end index");
    assert(endIndex < points.size() && "End index is larger than the number of points");
    assert(points.size() >= 2 && "At least two points needed");

    Vec2D lineDiff = points[endIndex] - points[startIndex];
    double lineLengthSquared = lineDiff.lengthSquared();

    if (lineLengthSquared == 0)
    {
        return findMostDistantPoint2D(points, startIndex, endIndex);
    }

    double maxDistanceSquared = 0.0;
    std::size_t maxDistanceIndex = startIndex;

    for (std::size_t i = startIndex + 1; i != endIndex; ++i)
    {
        double distanceSquared = point2LineDistanceSquared2D(points[i], points[startIndex],  points[endIndex]);
        if (distanceSquared > maxDistanceSquared)
        {
            maxDistanceIndex = i;
            maxDistanceSquared = distanceSquared;
        }
    }

    // Constructor is faster than initialization
    return std::make_pair(maxDistanceSquared, maxDistanceIndex);
}

void RamerDouglasPeucker2D(const std::vector<Point2D> &points, std::size_t startIndex,
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
        findMostDistantPointFromLine2D(points, startIndex, endIndex);

    if (maxDistanceSquared > epsilonSquared)
    {
        RamerDouglasPeucker2D(points, startIndex, maxDistanceIndex, epsilonSquared, indicesToKeep);
        RamerDouglasPeucker2D(points, maxDistanceIndex, endIndex, epsilonSquared, indicesToKeep);
    }
    else
    {
        // startIndex is included from the previous run because we execute sequentially with the
        // lower parts of the list first
        indicesToKeep.push_back(endIndex);
    }
}

} // end namespace rdp
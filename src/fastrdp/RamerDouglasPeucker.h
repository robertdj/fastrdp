#ifndef RDP
#define RDP

#include <vector>

namespace rdp {
struct Point3D
{
    double x;
    double y;
    double z;
};

void RamerDouglasPeucker(const std::vector<Point3D> &pointList, std::size_t startIndex,
                         std::size_t endIndex, double epsilonSquared,
                         std::vector<std::size_t> &indicesToKeep);
}

#endif

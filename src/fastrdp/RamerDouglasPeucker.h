#ifndef RDP
#define RDP

#include <vector>
#include "Geometry.h"

namespace rdp {

void RamerDouglasPeucker2D(const std::vector<Point2D> &pointList, std::size_t startIndex,
                         std::size_t endIndex, double epsilonSquared,
                         std::vector<std::size_t> &indicesToKeep);

}
#endif
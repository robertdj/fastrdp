#ifndef RDP
#define RDP

#include <vector>
#include "Geometry.h"

namespace rdp {

template <std::size_t N>
void RamerDouglasPeucker(const std::vector<Point<N>> &pointList, std::size_t startIndex,
                         std::size_t endIndex, double epsilonSquared,
                         std::vector<std::size_t> &indicesToKeep);

}
#endif
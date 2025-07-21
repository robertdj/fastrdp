#ifndef RDP
#define RDP

#include <array>
#include <vector>
#include "Geometry.h"

namespace rdp {

template <std::size_t N>
void RamerDouglasPeucker(const std::vector<Point<N>> &pointList, std::size_t startIndex,
                         std::size_t endIndex, double epsilonSquared,
                         std::vector<std::size_t> &indicesToKeep);

extern template void RamerDouglasPeucker<2>(const std::vector<Point<2>>&,
                std::size_t, std::size_t,
                double, std::vector<std::size_t>&);

extern template void RamerDouglasPeucker<3>(const std::vector<Point<3>>&,
                                            std::size_t, std::size_t,
                                            double, std::vector<std::size_t>&);

}


#endif

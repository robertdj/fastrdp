#ifndef RDP
#define RDP

#include <array>
#include <vector>
#include <cstddef>

namespace rdp {
template <std::size_t N>
struct Point {
    std::array<double, N> data{};

    Point() { data.fill(0.0); }
    Point(std::initializer_list<double> vals) {
        std::copy(vals.begin(), vals.end(), data.begin());
    }
};

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

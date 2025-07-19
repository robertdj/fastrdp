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
}

#endif

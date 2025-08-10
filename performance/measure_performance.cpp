#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../src/fastrdp/RamerDouglasPeucker.h"

int main(int argc, char* argv[])
{
    const std::size_t n = (argc > 1) ? static_cast<std::size_t>(std::atoll(argv[1])) : 10000000;
    printf("n: %zu \n", n);
    double epsilon = 0.5;

    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_real_distribution<double> u(-10.0, 10.0);

    std::vector<double> x(n);
    std::vector<double> y(n);
    for (size_t i = 0; i != n; i++) {
        x[i] = u(g);
        y[i] = u(g);
    }

    auto time_start_points = std::chrono::steady_clock::now();
    std::vector<rdp::Point<2>> points;
    points.reserve(n);
    for (size_t i = 0; i != n; i++) {
        points.push_back({ x[i], y[i] });
    }
    auto time_end_points = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration_points = time_end_points - time_start_points;
    printf("duration points: %g s\n", duration_points.count());

    std::vector<size_t> indicesToKeep;
    indicesToKeep.reserve(n);
    indicesToKeep.push_back(0);

    auto time_start = std::chrono::steady_clock::now();
    rdp::RamerDouglasPeucker(points, 0, n - 1, epsilon * epsilon, indicesToKeep);
    auto time_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = time_end - time_start;
    printf("duration RDP: %g s\n", duration.count());
}
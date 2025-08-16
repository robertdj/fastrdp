#include <cassert>
#include <variant>
#include <tuple>
#include <vector>
#include <array>
#include <cmath>
#include <cstddef>
#include <type_traits>

namespace rdp
{
    template <std::size_t N>
        requires (N > 1)
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

        const double& operator[](std::size_t i) const noexcept {
            return coords[i];
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

        const double& operator[](std::size_t i) const noexcept {
            return coords[i];
        }
    };


    template <std::size_t N>
    struct Subspace {
        Point<N> point;
        Vector<N> basis;
        std::conditional_t<(N > 2), Vector<N>, std::monostate> direction;

        Subspace(const Point<N>& p1, const Point<N>& p2)
        {
            point = p1;
            basis = Vector<N>(p1, p2);
            if constexpr (N > 2) {
                direction = basis.normalized2();
            }
        }

        bool is_null() const {
            return basis.abs2() == 0;
        }

        Vector<N> project(const Vector<N>& w) const
            requires (N > 2)
        {
            double dot_product = w.dot(direction);
            return basis.scale(dot_product);
        }

        double distance2(const Point<N>& p) const
            requires (N > 2)
        {
            auto w = Vector<N>(point, p);
            auto projected = project(w);
            return (w - projected).abs2();
        }

        // Faster formula in 2D:
        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
        // Note: This is without scaling by the length of basis.
        // This uses about half of the floating point ops and saves two temporary Vector allocations
        // compared to the general formula. On My Machine the general distance2 is about 50% slower
        // for N = 2
        double distance2(const Point<N>& p) const
            requires (N == 2)
        {
            double dist = basis[0] * (point[1] - p[1]) - basis[1] * (point[0] - p[0]);
            double dist2 = dist * dist;
            return dist2;
        }

        double abs2() const {
            return basis.abs2();
        }
    };


    // Find the point furthest away from reference (points[start_index] == points[end_index])
    template <std::size_t N>
    std::pair<double, std::size_t> find_most_distant_point(const std::vector<Point<N>>& points,
        std::size_t start_index, std::size_t end_index)
    {
        assert(start_index < end_index && "Start index must be smaller than end index");
        assert(end_index < points.size() && "End index is larger than the number of points");
        assert(points.size() >= 2 && "At least two points needed");

        assert(points[start_index] == points[end_index] && "Start and end point must be equal");

        const auto start_point = points[start_index];
        double max_dist2 = 0.0;
        std::size_t max_dist_index = start_index;

        for (std::size_t i = start_index + 1; i != end_index; ++i)
        {
            auto v = Vector<N>(start_point, points[i]);
            double dist2 = v.abs2();

            if (dist2 > max_dist2)
            {
                max_dist_index = i;
                max_dist2 = dist2;
            }
        }

        return std::make_pair(max_dist2, max_dist_index);
    }


    template <std::size_t N>
    std::pair<double, std::size_t> find_most_distant_point_from_line(const std::vector<Point<N>>& points, std::size_t start_index, std::size_t end_index)
    {
        assert(start_index < end_index && "Start index must be smaller than end index");
        assert(end_index < points.size() && "End index is larger than the number of points");
        assert(points.size() >= 2 && "At least two points needed");

        const auto start_point = points[start_index];
        const auto reference_space = Subspace(start_point, points[end_index]);

        if (reference_space.is_null())
        {
            return find_most_distant_point(points, start_index, end_index);
        }

        double max_dist2_a = 0.0;
        std::size_t max_dist_index_a = start_index;

        double max_dist2_b = 0.0;
        std::size_t max_dist_index_b = start_index;

        std::size_t i = start_index + 1;
        for (; i + 1 < end_index; i += 2)
        {
            double dist2a = reference_space.distance2(points[i]);
            if (dist2a > max_dist2_a)
            {
                max_dist2_a = dist2a;
                max_dist_index_a = i;
            }

            double dist2b = reference_space.distance2(points[i + 1]);
            if (dist2b > max_dist2_b)
            {
                max_dist2_b = dist2b;
                max_dist_index_b = i + 1;
            }
        }

        if (i < end_index)
        {
            double dist2 = reference_space.distance2(points[i]);
            if (dist2 > max_dist2_a)
            {
                max_dist2_a = dist2;
                max_dist_index_a = i;
            }
        }

        double max_dist2;
        std::size_t max_dist_index;
        if (max_dist2_a >= max_dist2_b)
        {
            max_dist2 = max_dist2_a;
            max_dist_index = max_dist_index_a;
        }
        else
        {
            max_dist2 = max_dist2_b;
            max_dist_index = max_dist_index_b;
        }

        if constexpr (N == 2) {
            const double line_length_squared = reference_space.abs2();
            max_dist2 /= line_length_squared;
        }

        return std::make_pair(max_dist2, max_dist_index);
    }


    template <std::size_t N>
    void ramer_douglas_peucker(const std::vector<Point<N>>& points, std::size_t start_index,
        std::size_t end_index, double epsilon_squared,
        std::vector<std::size_t>& indices_to_keep)
    {
        assert(start_index < end_index && "Start index must be smaller than end index");
        assert(end_index < points.size() && "End index is larger than the number of points");
        // The inequalities 0 <= startIndex < endIndex < points.size() imply that points.size() >= 2
        assert(points.size() >= 2 && "At least two points needed");

        assert(epsilon_squared >= 0 && "epsilon_squared must be non-negative");

        assert(indices_to_keep.size() >= 1 && "indices_to_keep should be non-empty");
        assert(indices_to_keep[0] == 0 && "indices_to_keep should be initialized with a 0");

        auto [max_dist2, max_dist_index] =
            find_most_distant_point_from_line(points, start_index, end_index);

        if (max_dist2 > epsilon_squared)
        {
            ramer_douglas_peucker(points, start_index, max_dist_index, epsilon_squared, indices_to_keep);
            ramer_douglas_peucker(points, max_dist_index, end_index, epsilon_squared, indices_to_keep);
        }
        else
        {
            // startIndex is included from the previous run because we execute sequentially with the
            // lower parts of the list first
            indices_to_keep.push_back(end_index);
        }
    }
} // end namespace rdp

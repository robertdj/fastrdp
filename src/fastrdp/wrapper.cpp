#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "RamerDouglasPeucker.h"

namespace py = pybind11;

std::vector<size_t> rdp_index(py::array_t<double> array1, py::array_t<double> array2, double epsilon)
{
    if (epsilon < 0.0)
        throw std::domain_error("epsilon must be non-negative");

    py::buffer_info buf1 = array1.request(), buf2 = array2.request();

    // Make sure the input arrays have the correct shape and data type
    if (buf1.ndim != 1 || buf2.ndim != 1)
        throw std::domain_error("Inputs should be vectors");

    auto n_points = buf1.size;
    if (n_points != buf2.size)
        throw std::length_error("Inputs have different lengths");

    if (n_points <= 2)
    {
        std::vector<size_t> trivial_indices(n_points);
        std::iota(trivial_indices.begin(), trivial_indices.end(), 0);
        return trivial_indices;
    }

    std::vector<double> vec1((double*)buf1.ptr, (double*)buf1.ptr + buf1.size);
    std::vector<double> vec2((double*)buf2.ptr, (double*)buf2.ptr + buf2.size);

    // Prepare input for RDP function
    std::vector<rdp::Point<2>> points;
    points.reserve(n_points);
    for (auto i = 0; i < n_points; i++)
        points.push_back({ vec1[i], vec2[i] });

    std::vector<size_t> indices_to_keep;
    indices_to_keep.reserve(n_points);
    indices_to_keep.push_back(0);

    rdp::ramer_douglas_peucker(points, 0, n_points - 1, epsilon * epsilon, indices_to_keep);

    return indices_to_keep;
}


py::array_t<size_t> rdp_index_wrapper(py::array_t<double> array1, py::array_t<double> array2, double epsilon)
{
    std::vector<size_t> indices_to_keep = rdp_index(array1, array2, epsilon);
    return py::array_t<size_t>(indices_to_keep.size(), indices_to_keep.data());
}


template <size_t N>
std::vector<rdp::Point<N>> parse_points(const py::array_t<double>& points) {
    py::buffer_info buf = points.request();

    if (buf.ndim != 2)
        throw std::domain_error("Input must be a 2D array");

    size_t n_rows = buf.shape[0];
    size_t n_cols = buf.shape[1];

    if (n_cols != N)
        throw std::domain_error("Expected each row to have " + std::to_string(N) + " columns");

    auto* data = static_cast<double*>(buf.ptr);

    std::vector<rdp::Point<N>> result;
    result.reserve(n_rows);
    std::array<double, N> row{};

    for (size_t i = 0; i < n_rows; ++i) {
        for (size_t j = 0; j < N; ++j) {
            row[j] = data[i * N + j];
        }
        result.push_back({ row });
    }

    return result;
}


std::vector<size_t> rdp_index_array(py::array_t<double> array, double epsilon)
{
    if (epsilon < 0.0)
        throw std::domain_error("epsilon must be non-negative");

    py::buffer_info buf = array.request();

    if (buf.ndim != 2)
        throw std::domain_error("Input must be a 2D array");

    const size_t n_cols = buf.shape[1];

    if (n_cols != 2 && n_cols != 3)
        throw std::domain_error("Input must have 2 or 3 columns");

    const size_t n_points = buf.shape[0];

    if (n_points <= 2)
    {
        std::vector<size_t> trivial_indices(n_points);
        std::iota(trivial_indices.begin(), trivial_indices.end(), 0);
        return trivial_indices;
    }

    std::vector<size_t> indices_to_keep;
    indices_to_keep.reserve(n_points);
    indices_to_keep.push_back(0);

    if (n_cols == 2) {
        auto points = parse_points<2>(array);
        rdp::ramer_douglas_peucker(points, 0, n_points - 1, epsilon * epsilon, indices_to_keep);
    }
    else if (n_cols == 3) {
        auto points = parse_points<3>(array);
        rdp::ramer_douglas_peucker(points, 0, n_points - 1, epsilon * epsilon, indices_to_keep);
    }
    else {
        throw std::domain_error("Points must have 2 or 3 columns");
    }

    return indices_to_keep;
}


py::array_t<size_t> rdp_index_wrapper_array(py::array_t<double> array, double epsilon)
{
    std::vector<size_t> indices_to_keep = rdp_index_array(array, epsilon);
    return py::array_t<size_t>(indices_to_keep.size(), indices_to_keep.data());
}


PYBIND11_MODULE(_fastrdp, m)
{
    m.def("rdp_index", &rdp_index_wrapper, R"mydelimiter(
        rdp_index(x, y, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Returns the indices of the elements that are kept in an approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");
    m.def("rdp_index", &rdp_index_wrapper_array, R"mydelimiter(
        rdp_index(X, epsilon)

    `X` are samples from an `N` dimensional curve, with each row being a sample.
    Returns the indices of the elements that are kept in an approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");
}

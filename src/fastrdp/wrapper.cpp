#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "RamerDouglasPeucker.h"

namespace py = pybind11;

template <std::size_t N>
std::vector<std::size_t>
rdp_index(const std::array<py::array_t<double>, N> &arrays, double epsilon)
{
    if (epsilon < 0.0)
        throw std::domain_error("epsilon must be non-negative");

    // py::buffer_info buf1 = array1.request(), buf2 = array2.request(), buf3 = array3.request();
    std::array<py::buffer_info, N> buf;
    for (std::size_t k = 0; k < N; ++k)
        buf[k] = arrays[k].request();

    // Make sure the input arrays have the correct shape and data type
    auto nPoints = buf[0].size;
    for (std::size_t i = 0; i < N; ++i)
        if (buf[i].ndim != 1)
            throw std::invalid_argument("Inputs should be vectors");

        if (i > 0 && bufs[i].size != bufs[0].size)
            throw std::length_error("Inputs have different lengths");
    if (nPoints <= 2)
    {
        std::vector<size_t> trivial_indices(nPoints);
        std::iota(trivial_indices.begin(), trivial_indices.end(), 0);
        return trivial_indices;
    }

    // Prepare input for RDP function
    std::vector<rdp::Point<N>> points;
    points.reserve(nPoints);
    for (std::size_t i = 0; i < nPoints; ++i)
    {
        rdp::Point<N> p;
        for (std::size_t k = 0; k < N; ++k)
            p.data[k] = static_cast<double *>(buf[k].ptr)[i];
        points.push_back(p);
    }

    std::vector<size_t> indicesToKeep;
    indicesToKeep.reserve(nPoints);
    indicesToKeep.push_back(0);

    rdp::RamerDouglasPeucker(points, 0, nPoints - 1, epsilon * epsilon, indicesToKeep);

    return indicesToKeep;
}

template <std::size_t N>
py::array_t<std::size_t>
rdp_index_wrapper(const std::array<py::array_t<double>, N> &arrays, double epsilon)
{
    std::vector<size_t> indicesToKeep = rdp_index<N>(arrays, epsilon);
    return py::array_t<size_t>(indicesToKeep.size(), indicesToKeep.data());
}

template <std::size_t N>
py::tuple rdp_wrapper(const std::array<py::array_t<double>, N> &arrays, double epsilon)
{
    std::vector<size_t> indicesToKeep = rdp_index<N>(arrays, epsilon);

    std::array<py::buffer_info, N> buf;
    for (std::size_t k = 0; k < N; ++k)
        buf[k] = arrays[k].request();

    size_t nIndices = indicesToKeep.size();
    std::array<std::vector<double>, N> coordsOut;
    for (size_t k = 0; k < N; ++k)
        coordsOut[k].resize(nIndices);

    // Output arrays
    for (size_t i = 0; i < nIndices; ++i)
    {
        size_t idx = indicesToKeep[i];
        for (size_t k = 0; k < N; ++k)
            coordsOut[k][i] = static_cast<double *>(buf[k].ptr)[idx];
    }

    py::tuple result(N);
    for (size_t k = 0; k < N; ++k)
        result[k] = py::array(coordsOut[k].size(), coordsOut[k].data());

    return result;
}


py::array_t<std::size_t> rdp_index_wrapper_2d(py::array_t<double> x, py::array_t<double> y, double epsilon) {
    return rdp_index_wrapper<2>({x, y}, epsilon);
}

py::array_t<std::size_t> rdp_index_wrapper_3d(py::array_t<double> x, py::array_t<double> y, py::array_t<double> z, double epsilon) {
    return rdp_index_wrapper<3>({x, y, z}, epsilon);
}

py::tuple rdp_wrapper_2d(py::array_t<double> x, py::array_t<double> y, double epsilon) {
    return rdp_wrapper<2>({x, y}, epsilon);
}

py::tuple rdp_wrapper_3d(py::array_t<double> x, py::array_t<double> y, py::array_t<double> z, double epsilon) {
    return rdp_wrapper<3>({x, y, z}, epsilon);
}

PYBIND11_MODULE(_fastrdp, m)
{
    m.def("rdp_index", &rdp_index_wrapper_2d, R"mydelimiter(
        rdp_index(x, y, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Returns the indices of the elements that are kept in an approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");

    m.def("rdp_index", &rdp_index_wrapper_3d, R"mydelimiter(
        rdp_index(x, y, z, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x`, `y`, and `z`.
    Returns the indices of the elements that are kept in an approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");

    m.def("rdp", &rdp_wrapper_2d, R"mydelimiter(
        rdp(x, y, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Select a subset of the points as a coarser approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");

    m.def("rdp", &rdp_wrapper_3d, R"mydelimiter(
        rdp(x, y, z, epsilon)

    The input is a curve sampled at the points `(x[i], y[i], z[i])` from `x`, `y` and `z`.
    Select a subset of the points as a coarser approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");
}

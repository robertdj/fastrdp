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

    auto nPoints = buf1.size;
    if (nPoints != buf2.size)
        throw std::length_error("Inputs have different lengths");

    if (nPoints <= 2)
    {
        std::vector<size_t> trivial_indices(nPoints);
        std::iota(trivial_indices.begin(), trivial_indices.end(), 0);
        return trivial_indices;
    }

    std::vector<double> vec1((double*)buf1.ptr, (double*)buf1.ptr + buf1.size);
    std::vector<double> vec2((double*)buf2.ptr, (double*)buf2.ptr + buf2.size);

    // Prepare input for RDP function
    std::vector<rdp::Point<2>> points;
    points.reserve(nPoints);
    for (auto i = 0; i < nPoints; i++)
        points.push_back({ vec1[i], vec2[i] });

    std::vector<size_t> indicesToKeep;
    indicesToKeep.reserve(nPoints);
    indicesToKeep.push_back(0);

    rdp::RamerDouglasPeucker(points, 0, nPoints - 1, epsilon * epsilon, indicesToKeep);

    return indicesToKeep;
}

py::array_t<size_t> rdp_index_wrapper(py::array_t<double> array1, py::array_t<double> array2, double epsilon)
{
    std::vector<size_t> indicesToKeep = rdp_index(array1, array2, epsilon);
    return py::array_t<size_t>(indicesToKeep.size(), indicesToKeep.data());
}

std::pair<py::array_t<double>, py::array_t<double>> rdp_wrapper(py::array_t<double> array1, py::array_t<double> array2, double epsilon)
{
    std::vector<size_t> indicesToKeep = rdp_index(array1, array2, epsilon);

    py::buffer_info buf1 = array1.request(), buf2 = array2.request();
    std::vector<double> vec1((double*)buf1.ptr, (double*)buf1.ptr + buf1.size);
    std::vector<double> vec2((double*)buf2.ptr, (double*)buf2.ptr + buf2.size);

    size_t nIndices = indicesToKeep.size();
    std::vector<double> xOut(nIndices);
    std::vector<double> yOut(nIndices);

    for (size_t i = 0; i < nIndices; ++i)
    {
        size_t index = indicesToKeep[i];
        xOut[i] = vec1[index];
        yOut[i] = vec2[index];
    }

    return std::make_pair(py::array(xOut.size(), xOut.data()), py::array(yOut.size(), yOut.data()));
}

PYBIND11_MODULE(_fastrdp, m)
{
    m.def("rdp_index", &rdp_index_wrapper, R"mydelimiter(
        rdp_index(x, y, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Returns the indices of the elements that are kept in an approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");
    m.def("rdp", &rdp_wrapper, R"mydelimiter(
        rdp(x, y, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Select a subset of the points as a coarser approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");
}

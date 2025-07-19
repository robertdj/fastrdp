#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "RamerDouglasPeucker.h"

namespace py = pybind11;

// std::vector<size_t> rdp_index(py::array_t<double> array1, py::array_t<double> array2, py::array_t<double> array3, double epsilon)
// {
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
    if (buf1.ndim != 1 || buf2.ndim != 1 || buf3.ndim != 1)
        throw std::domain_error("Inputs should be vectors");

    auto nPoints = buf[0].size;
    for (std::size_t i = 1; i < N; ++i)
        if (nPoints != buf[i].size)
            throw std::length_error("Inputs have different lengths");
    // if (nPoints != buf2.size || nPoints != buf3.size)
    //     throw std::length_error("Inputs have different lengths");

    if (nPoints <= 2)
    {
        std::vector<size_t> trivial_indices(nPoints);
        std::iota(trivial_indices.begin(), trivial_indices.end(), 0);
        return trivial_indices;
    }

    // std::vector<double> vec1((double *)buf1.ptr, (double *)buf1.ptr + buf1.size);
    // std::vector<double> vec2((double *)buf2.ptr, (double *)buf2.ptr + buf2.size);
    // std::vector<double> vec3((double *)buf3.ptr, (double *)buf3.ptr + buf3.size);
    std::vector<rdp::Point<N>> points;

    // Prepare input for RDP function
    std::vector<rdp::Point<N>> points;
    points.reserve(nPoints);
    // for (auto i = 0; i < nPoints; i++)
    //     points.push_back({vec1[i], vec2[i], vec3[i]});
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



// py::array_t<size_t> rdp_index_wrapper(py::array_t<double> array1, py::array_t<double> array2, py::array_t<double> array3, double epsilon)
// {
template <std::size_t N>
py::array_t<std::size_t>
rdp_index_wrapper(const std::array<py::array_t<double>, N> &arrays, double epsilon)
{
    std::vector<size_t> indicesToKeep = rdp_index<N>(arrays, epsilon);
    return py::array_t<size_t>(indicesToKeep.size(), indicesToKeep.data());
}

std::tuple<py::array_t<double>, py::array_t<double>, py::array_t<double>> rdp_wrapper(py::array_t<double> array1, py::array_t<double> array2, py::array_t<double> array3, double epsilon)
{
    std::vector<size_t> indicesToKeep = rdp_index(array1, array2, array3, epsilon);

    py::buffer_info buf1 = array1.request(), buf2 = array2.request(), buf3 = array3.request();
    std::vector<double> vec1((double *)buf1.ptr, (double *)buf1.ptr + buf1.size);
    std::vector<double> vec2((double *)buf2.ptr, (double *)buf2.ptr + buf2.size);
    std::vector<double> vec3((double *)buf3.ptr, (double *)buf3.ptr + buf3.size);

    size_t nIndices = indicesToKeep.size();
    std::vector<double> xOut(nIndices);
    std::vector<double> yOut(nIndices);
    std::vector<double> zOut(nIndices);

    for (size_t i = 0; i < nIndices; ++i)
    {
        size_t index = indicesToKeep[i];
        xOut[i] = vec1[index];
        yOut[i] = vec2[index];
        zOut[i] = vec3[index];
    }

    // return std::make_pair(py::array(xOut.size(), xOut.data()), py::array(yOut.size(), yOut.data()));
    return std::make_tuple(
    py::array(xOut.size(), xOut.data()),
    py::array(yOut.size(), yOut.data()),
    py::array(zOut.size(), zOut.data())
    );

}

PYBIND11_MODULE(_fastrdp, m)
{
    m.def("rdp_index", &rdp_index_wrapper, R"mydelimiter(
        rdp_index(x, y, z, epsilon)

    The input is a curve sampled at the points `(x[i], y[i])` from `x` and `y`.
    Returns the indices of the elements that are kept in an approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");
    m.def("rdp", &rdp_wrapper, R"mydelimiter(
        rdp(x, y, z, epsilon)

    The input is a curve sampled at the points `(x[i], y[i], z[i])` from `x` and `y` and `z`.
    Select a subset of the points as a coarser approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");
}

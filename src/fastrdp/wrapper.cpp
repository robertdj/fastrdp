#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "RamerDouglasPeucker.h"

namespace py = pybind11;

std::pair<py::array_t<double>, py::array_t<double>> rdp_wrapper(py::array_t<double> array1, py::array_t<double> array2, double epsilon)
{
    if (epsilon < 0.0)
    {
        throw std::domain_error("epsilon must be non-negative");
    }

    // Convert Python numpy arrays to C++ vectors
    auto ptr1 = array1.request();
    auto ptr2 = array2.request();

    // Make sure the input arrays have the correct shape and data type
    // if (array1.ndim != 1 || array2.ndim != 1)
    //     throw std::domain_error("Inputs should be vectors");

    py::ssize_t nPoints = ptr1.size;
    if (nPoints != ptr2.size)
    {
        throw std::length_error("Inputs have different lengths");
    }

    if (nPoints < 2)
    {
        return std::make_pair(array1, array2);
    }

    std::vector<double> vec1((double *)ptr1.ptr, (double *)ptr1.ptr + ptr1.size);
    std::vector<double> vec2((double *)ptr2.ptr, (double *)ptr2.ptr + ptr2.size);

    // Prepare input for RDP function
    std::vector<rdp::Point2D> points;
    points.reserve(nPoints);
    for (auto i = 0; i < nPoints; i++)
    {
        points.push_back({vec1[i], vec2[i]});
    }

    std::vector<size_t> indicesToKeep;
    indicesToKeep.reserve(nPoints);
    indicesToKeep.push_back(0);

    rdp::RamerDouglasPeucker(points, 0, nPoints - 1, epsilon * epsilon, indicesToKeep);

    // Create arrays to return
    size_t nIndices = indicesToKeep.size();
    std::vector<double> xOut(nIndices);
    std::vector<double> yOut(nIndices);

    for (size_t i = 0; i < nIndices; ++i)
    {
        size_t index = indicesToKeep[i];
        xOut[i] = vec1[index];
        yOut[i] = vec2[index];
    }

    // Convert C++ vectors to NumPy arrays and return as a pair
    return std::make_pair(py::array(xOut.size(), xOut.data()), py::array(yOut.size(), yOut.data()));
}

// Define the pybind11 module
PYBIND11_MODULE(fastrdp, m)
{
    m.def("rdp", &rdp_wrapper, R"mydelimiter(
        rdp(x, y, epsilon)

        The input is a curve sampled at the points `(x[i], y[i])` from NumPy vectors `x` and `y`.
        Select a subset of the points as a coarser approximation using the Ramer-Douglas-Peucker algorithm with tolerance `epsilon`.
)mydelimiter");
}

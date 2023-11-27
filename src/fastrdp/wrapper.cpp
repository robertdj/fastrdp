#include <Python.h>
#include <numpy/arrayobject.h>
#include "RamerDouglasPeucker.h"


int check_numpy_array(PyObject* obj) {
    if (!PyArray_Check(obj)) {
        PyErr_SetString(PyExc_TypeError, "Input must be a Numpy array");
        return 0;
    }

    if (PyArray_TYPE((PyArrayObject *)obj) != NPY_FLOAT64) {
        PyErr_SetString(PyExc_TypeError, "Input must be a Numpy array of type float");
        return 0;
    }

    return 1;
}


static PyObject* rdp_wrapper(PyObject* self, PyObject* args) {
    PyObject* arr1_obj;
    PyObject* arr2_obj;
    double epsilon;
    
    if (!PyArg_ParseTuple(args, "OOd", &arr1_obj, &arr2_obj, &epsilon)) {
        return NULL;
    }

    if (epsilon < 0.0) {
        PyErr_SetString(PyExc_ValueError, "epsilon must be non-negative");
        return NULL;
    }

    if (!check_numpy_array(arr1_obj)) {
        return NULL;
    }

    if (!check_numpy_array(arr2_obj)) {
        return NULL;
    }

    PyArrayObject* arr1 = reinterpret_cast<PyArrayObject*>(arr1_obj);
    PyArrayObject* arr2 = reinterpret_cast<PyArrayObject*>(arr2_obj);

    npy_intp nPoints = PyArray_SIZE(arr1);

    if (nPoints != PyArray_SIZE(arr2)) {
        PyErr_SetString(PyExc_ValueError, "Inputs have different lengths");
        return NULL;
    }

    if (nPoints <= 2) {
        return Py_BuildValue("OO", arr1_obj, arr2_obj);
    }

    double* data1 = static_cast<double*>(PyArray_DATA(arr1));
    double* data2 = static_cast<double*>(PyArray_DATA(arr2));

    std::vector<rdp::Point2D> points;
    points.reserve(nPoints);
    for (npy_intp i = 0; i < nPoints; i++) {
        points.push_back({data1[i], data2[i]});
    }

    // Create vectors from the input NumPy arrays
    std::vector<size_t> indicesToKeep;
    indicesToKeep.reserve(nPoints);
    indicesToKeep.push_back(0);

    rdp::RamerDouglasPeucker(points, 0, nPoints - 1, epsilon * epsilon, indicesToKeep);

    // Create new NumPy arrays to return
    size_t nIndices = indicesToKeep.size();

    npy_intp dims[1] = {static_cast<npy_intp>(nIndices)};

    PyObject* result1_obj = PyArray_FROM_OTF(PyArray_SimpleNew(1, dims, NPY_DOUBLE), NPY_DOUBLE, NPY_ARRAY_FORCECAST);
    PyObject* result2_obj = PyArray_FROM_OTF(PyArray_SimpleNew(1, dims, NPY_DOUBLE), NPY_DOUBLE, NPY_ARRAY_FORCECAST);

    if (!result1_obj || !result2_obj) {
        Py_XDECREF(result1_obj);
        Py_XDECREF(result2_obj);
        return NULL;
    }

    double* result1_data = static_cast<double*>(PyArray_DATA(reinterpret_cast<PyArrayObject*>(result1_obj)));
    double* result2_data = static_cast<double*>(PyArray_DATA(reinterpret_cast<PyArrayObject*>(result2_obj)));

    for (size_t i = 0; i < nIndices; ++i) {
        size_t index = indicesToKeep[i];
        result1_data[i] = data1[index];
        result2_data[i] = data2[index];
    }

    return Py_BuildValue("OO", result1_obj, result2_obj);
}

// Documentation string
PyDoc_STRVAR(rdp_doc, "rdp(arr1, arr2, epsilon)\n\n\
Compute approximation using the Ramer-Douglas-Peucker algorithm");

static PyMethodDef module_methods[] = {
    {"rdp", rdp_wrapper, METH_VARARGS, rdp_doc},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "fastrdp",
    NULL,
    -1,
    module_methods
};

PyMODINIT_FUNC PyInit_fastrdp(void) {
    // Initialize NumPy
    import_array(); 
    return PyModule_Create(&module_def);
}

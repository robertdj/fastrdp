#include <Python.h>
#include <numpy/arrayobject.h>

double mysum(size_t n, double *X) {
    double s = 0.0;
    for (size_t i = 0; i < n; ++i) {
        s += X[i];
    }
    return s;
}

static PyObject* mysum_wrapper(PyObject* self, PyObject* args) {
    PyObject* arr_obj;
    if (!PyArg_ParseTuple(args, "O", &arr_obj)) {
        return NULL;
    }

    PyArrayObject* arr = reinterpret_cast<PyArrayObject*>(arr_obj);
    if (!PyArray_ISCARRAY(arr)) {
        PyErr_SetString(PyExc_TypeError, "Input array must be contiguous C-style");
        return NULL;
    }

    double* data = static_cast<double*>(PyArray_DATA(arr));
    npy_intp n = PyArray_SIZE(arr);

    double result = mysum(n, data);
    return PyFloat_FromDouble(result);
}

static PyMethodDef module_methods[] = {
    {"mysum", mysum_wrapper, METH_VARARGS, "Calculate the sum of elements in a NumPy array."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "RDP",
    NULL,
    -1,
    module_methods
};

PyMODINIT_FUNC PyInit_RDP(void) {
    import_array();  // Initialize NumPy
    return PyModule_Create(&module_def);
}


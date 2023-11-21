from setuptools import Extension, find_packages, setup

# https://stackoverflow.com/questions/19919905/how-to-bootstrap-numpy-installation-in-setup-py/21621689
class get_numpy_include(object):
    """Defer numpy.get_include() until after numpy is installed."""

    def __str__(self):
        import numpy
        return numpy.get_include()


module = Extension(
    name="fastrdp", 
    sources=["src/fastrdp/wrapper.cpp", "src/fastrdp/RamerDouglasPeucker.cpp"],
    include_dirs=[get_numpy_include()],
    extra_compile_args=["-std=c++17"],
    define_macros=[("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")]
)

setup(
    packages=find_packages("src"),
    package_dir={"": "src"},
    ext_modules=[module],
    setup_requires=['numpy']
)

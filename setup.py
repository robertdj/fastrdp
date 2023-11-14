from setuptools import Extension, find_packages, setup
from numpy import get_include


module = Extension(
    "RDP", 
    sources=["src/PyRDP/wrapper.cpp", "src/PyRDP/RamerDouglasPeucker.cpp"],
    include_dirs=[get_include()],
    extra_compile_args=["-std=c++17"],
    define_macros=[("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")]
)

setup(
    name="RDP",
    version="0.0.1",
    author="Robert Dahl Jacobsen",
    description="Implementation of the Ramer-Douglas-Peucker algorithm",
    license="MIT",
    packages=find_packages("src"),
    package_dir={"": "src"},
    ext_modules=[module],
    install_requires=['numpy']
)

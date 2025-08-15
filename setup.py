from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

module = Pybind11Extension("_fastrdp", ["src/fastrdp/wrapper.cpp"], cxx_std=20)

setup(
    ext_modules=[module],
    cmdclass={"build_ext": build_ext},
)

from distutils.core import setup

from Cython.Build import cythonize

setup(
    ext_modules=cythonize("_find_contours_cy.pyx")
)

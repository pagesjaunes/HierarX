from setuptools import setup, find_packages, dist
import os
dist.Distribution().fetch_build_eggs(['Cython'])

from distutils.extension import Extension

try:
    long_description = open("README.rst").read()
except IOError:
    long_description = ""

USE_CYTHON = True

sources = ['pyhierarx/cy/HierarXbinder.pyx']
if USE_CYTHON:
    from Cython.Distutils import build_ext
    from Cython.Build import cythonize

    extensions = cythonize([
        Extension(
            'pyhierarx.cy',
            sources=sources,
            extra_compile_args=['-std=gnu++11'],
            language='c++',
            include_dirs=[
                os.path.join(os.environ['PWD'], "src"),
                os.environ['ARGH_HOME'],
                os.path.join(os.environ['ANNOY_HOME'], "src")
            ]
        )
    ])
else:
    extensions = [
        Extension(
            "pyhierarx.cy",
            sources=["%s.cpp" % x.split('.')[0] for x in sources]
        )
    ]

setup(
    name="pyhierarx",
    version="0.1",
    description="Various embeddings implementation and evaluation",
    license="MIT",
    author="ftorregrossa",
    packages=find_packages(),
    package_data={},
    install_requires=[],
    long_description=long_description,
    ext_modules=extensions,
    classifiers=[
        "Programming Language :: Python",
        "Programming Language :: Python :: 3.6",
    ],
    cmdclass = {'build_ext': build_ext} if USE_CYTHON else {}
)

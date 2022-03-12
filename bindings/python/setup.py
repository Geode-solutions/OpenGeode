# Copyright (c) 2019 - 2022 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the 'Software'), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

from setuptools import setup, Extension
from setuptools.dist import Distribution
from os import path

class BinaryDistribution(Distribution):
    def has_ext_modules(foo):
        return True

with open(path.join('${CMAKE_SOURCE_DIR}', 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name='OpenGeode-core',
    version='${WHEEL_VERSION}',
    description='Open source framework for representing and manipulating geometric models',
    keywords=['brep', 'modeling', 'mesh', 'meshing'],
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='Geode-solutions',
    author_email='contact@geode-solutions.com',
    url='https://github.com/Geode-solutions/OpenGeode',
    packages=['opengeode'],
    package_data={
        'opengeode': ['*.so', '*.dll', '*.pyd', '*.dylib']
    },
    license='MIT',
    classifiers=[
        'License :: OSI Approved :: MIT License'
    ],
    zip_safe=False,
    distclass=BinaryDistribution
)
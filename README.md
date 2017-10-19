# lib
My personal collection of useful C++ containers and function templates.

## Building and Testing
This project uses CMake, the tests can be built with a C++14 compatible
compiler.
```
mkdir build && cd build
cmake ..
make test
```
The unit tests require [Boost.Test](http://www.boost.org/doc/libs/1_63_0/libs/test/doc/html/index.html).
Eventually the tests will run with [gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) on some sort of continuous integration server.
I suggest running the test executables with [perf(1)](http://man7.org/linux/man-pages/man1/perf.1.html).

## Contents

### Decorators
A generic decorator function template which wraps any function with a
begin and end hook.
This is particularly useful if you want to time a function.
The goal is to make it usable for memoization, similar to
[Python Decorators](https://wiki.python.org/moin/PythonDecorators).

### List
Essentially an implementation of the STL list.
Allocator aware, and supports the latest standard's api.
Implemented as a doubly linked, circular linked list with a single
sentinel node.
In theory, it can be used in place of std::list.
Still needs *alot* more tests.

### Point
An n-dimensional point class template.
The idea here is to make common operations on points less prone to
errors.
The header includes orthogonal projections, euclidean distance,
Manhattan distance, and some common operators.

### Vector
STL vector implementation.

## License
This project is licensed under the MIT License.
See the license text in [LICENSE.md](LICENSE.md).

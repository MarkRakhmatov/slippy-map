# slippy-map
[![Build Status](https://github.com/MarkRakhmatov/slippy-map/actions/workflows/tests.yml/badge.svg)](https://github.com/MarkRakhmatov/slippy-map/actions/workflows/tests.yml)

Simple C++ slippy map

## Dependencies
* Catch2
* SDL3
* SDL3_Image

## Build
```
cmake -S . -B build
cmake --build build
```

## Test coverage
### Windows local coverage setup
Prerequisites:
* Install OpenCppCoverage
* Add OpenCppCoverage to PATH environment variable
* build tests

OpenCppCoverage.exe --sources=src\tiles --export_type cobertura:coverage/coverage.cobertura -- tests\build\Debug\tests.exe
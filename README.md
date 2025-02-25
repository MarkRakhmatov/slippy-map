# slippy-map
[![Build Status](https://github.com/MarkRakhmatov/slippy-map/actions/workflows/tests.yml/badge.svg)](https://github.com/MarkRakhmatov/slippy-map/actions/workflows/tests.yml)
<a href="https://scan.coverity.com/projects/markrakhmatov-slippy-map">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/31418/badge.svg"/>
</a>

https://github.com/user-attachments/assets/12188e03-e61f-400f-8954-32298ddd5ddd

Simple C++ slippy map

## Dependencies
* SDL3
* SDL3_Image
* CURL
* Catch2

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

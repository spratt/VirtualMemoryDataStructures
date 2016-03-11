#!/bin/bash
make distclean
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..

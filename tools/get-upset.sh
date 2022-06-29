#!/bin/bash
cd "$(dirname $(readlink -f $0))"
git clone https://github.com/pocomane/upset.git
cd upset
g++ -Og upset.cpp -I. -o upset

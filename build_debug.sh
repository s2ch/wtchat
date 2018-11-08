#!/bin/sh

cd "$(dirname "$0")"
rm -rf build/debug
meson --buildtype debug -Dwtroot=/opt/build/wt4 build/debug
cd build/debug
ninja

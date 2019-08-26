#!/usr/bin/env bash

for i in *; do
  cd "$i"
  echo "Testing $i"
  make clean
  make -j
  ./main
  cd ..
done

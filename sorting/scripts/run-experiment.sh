#!/bin/bash

set -e
file='final-results-sorted.txt'
#rm -f final-results.txt

echo $'Seed\tCores\tSize\tTime' > $file
for seed in $(seq -1 -1 -5); do
  for size in $(seq 10000 50000 500000); do
    for cores in 1 2 4; do
      echo -n "$seed\t$cores\t$size\t" >> $file
      mpiexec -n $cores ../cmake-build-debug/sorting $seed $size >> $file
    done
  done
done

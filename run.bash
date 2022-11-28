#!/usr/bin/env bash

num_processor=$(grep -c ^processor /proc/cpuinfo)
#num_processor=7
width=2048
rep=1
gcc main.c -fopenmp || exit

echo "" >>output.txt
echo "$width x $width" >>output.txt

for i in $(seq 1 "$num_processor"); do
  echo "Welcome $i x$rep times"
  printf '%d ' "$i" >>output.txt

  for j in $(seq 1 "$rep"); do
    ./a.out "$width" "$i" 2>>output.txt
  done

  echo "" >>output.txt

done

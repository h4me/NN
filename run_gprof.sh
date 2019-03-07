#!/bin/bash


./bin/mnist_loader

if [ ! -f "gmon.out" ]; then
   echo "ERROR : gmon.out does not exist"
   exit
fi


gprof ./bin/mnist_loader gmon.out > mnist_analysis.txt




 

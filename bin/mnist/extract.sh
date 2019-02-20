#!/bin/bash

list="t10k-images-idx3-ubyte.gz t10k-labels-idx1-ubyte.gz train-images-idx3-ubyte.gz train-labels-idx1-ubyte.gz"

for item in $list; do
 
   zcat $item > $item.raw

done



#zcat train-images-idx3-ubyte.gz > train-images-idx3-ubyte.data
#zcat train-labels-idx1-ubyte.gz > train-labels-idx1-ubyte.data
#zcat t10k-labels-idx1-ubyte.gz > t10k-labels-idx1-ubyte.data
#zcat     t10k-labels-idx1-ubyte.data


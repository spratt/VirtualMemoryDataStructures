# VirtualMemoryDataStructures

Compares various data structures often used in virtual memory systems.

In particular, compares red-black trees, AVL trees, splay trees, and
doubly-linked lists.

To build: make

To run: make run

## Results

The following tests were performed on a 2.7 GHz Intel Core i7, 16 GB
1600 MHz DDR3, running Mac OS X 10.9.5.

With 10000 elements, 4 repetitions

    Container Red-Black Tree
    Insert ns/iter: 964.35
    Search ns/iter: 742.925
    
    Container AVL Tree
    Insert ns/iter: 1039.17
    Search ns/iter: 701.525

With 1000000 elements, 4 repetitions

    Container Red-Black Tree
    Insert ns/iter: 1831.02
    Search ns/iter: 1586.56
    
    Container AVL Tree
    Insert ns/iter: 1879.46
    Search ns/iter: 1510.27

With 1000000 elements, 30 repetitions

    Container Red-Black Tree
    Insert ns/iter: 1860.15
    Search ns/iter: 1597.42
    
    Container AVL Tree
    Insert ns/iter: 1931.18
    Search ns/iter: 1515.38

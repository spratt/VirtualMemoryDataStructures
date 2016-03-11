# VirtualMemoryDataStructures

Compares various data structures often used in virtual memory systems.

In particular, compares red-black trees, AVL trees, splay trees, and
doubly-linked lists.

To build: make

To run: make run

## Results

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

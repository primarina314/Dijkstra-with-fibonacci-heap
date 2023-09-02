# Dijkstra-with-fibonacci-heap
This is an implementation of Dijkstra's algorithm using fibonacci-heap. Fibonacci heap is implemented using some smart pointers.

# Environment
> WSL2 Linux

> g++ (Ubuntu 11.3.0-1ubuntu1~22.04.1) 11.3.0

> g++ -o main main.cpp

I confirmed that no memory leak occurred using the following command.
> valgrind --leak-check=yes ./main

> valgrind --leak-check=yes --leak-check=full --show-leak-kinds=all ./main

Thank you.
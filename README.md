# mpi-distributed-integral
Program to calc integrals in a distributed way using MPI lib.
This project is, actually, a college project for Distributed Systems.

### Statement
The program must calculate the integral in the interval [0, 100], and it must use a master-slave structure (the inverse
of client-server) using **MPI**. The master program must request each slave to calculate part of the interval, hoping 
that they will return their part, which will be accumulated for the final result by the master.

The program must be run for the following conditions:
 
- Number of slaves ranging from 1, 2, 4 and 10 slaves;
- Discretion range being 0.0001, 0.00001 and 0.000001.

## Install guide

    Please, run this program using a Unix environment!

### C compiler (**GCC**)
First, be sure that you have **GCC** installed. In a terminal, type:
```
> gcc -v
```

The output should look like the following:
```
...
gcc version 9.3.0 (Ubuntu 9.3.0-10ubuntu2)
```

### **MPI** lib
Then, install **MPI** using the following:
```
> apt-get install libmpich-dev
```

You can check the installation by typing:
```
> mpicc -v
```

The output should look like the following:
```
mpicc for MPICH version 3.3.2
...
```

## Running the project
In your terminal, navigate to the project directory. Then, compile it with:
```
> mpicc src/main.c -o main.e -lm
```

To test it by the statement's conditions, pass the number of nodes as the first `mpirun` argument, using `-n` parameter (**don't forget to consider the master node**) and the discretization value as the second argument. Thus, follow the following structure:
```
> mpirun -n <numberOfNodes> ./main.e <discretizationValue>
```

- `numberOfNodes`: this argument represents the number of nodes that MPI will create to do the processing. **It considers the master node**, so, to work with 4 slaves, for example, you must pass 5 as this argument.
- `discretizationValue`: represents the discretization value that the slaves nodes will consider to calc their interval.
  
For instance, bellow command will calc the integral with **4 slaves** and a **0.0001 discretization** value:
```
> mpirun -n 5 ./main.e 0.0001
```

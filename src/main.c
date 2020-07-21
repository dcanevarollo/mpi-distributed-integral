#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpich/mpi.h>

#define MASTER 0 // Rank 0 is the master node
#define MAX_INTERVAL 100 

double calcInterval(double initialInterval, int subintervalSize, double discretization) {
  double result, point1, point2, value;

  result = 0;
  for (value = initialInterval; value < subintervalSize; value += discretization) {
    double a = value;
    double b = a + discretization;

    point1 = sqrt(pow(100, 2) - pow(a, 2));
    point2 = sqrt(pow(100, 2) - pow(b, 2));

    result += (point1 + point2) * discretization / 2;
  }

  return result;
}

void validateEntry(int numberOfNodes) {
  if (
    numberOfNodes != 2 && 
    numberOfNodes != 3 && 
    numberOfNodes != 5 && 
    numberOfNodes != 11
  ) {
    printf("The number of nodes allowed is 2, 3, 5 or 10\n");
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int size, rank, nproc, subintervalSize;
  double discretization, initialInterval, calculatedValue, total;
  MPI_Status status;

  // The discretization value is passed as command line argument
  discretization = strtold(argv[1], NULL);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  validateEntry(size);

  subintervalSize = MAX_INTERVAL / (size - 1); // Excludes the master node

  if (rank == MASTER) {
    for (nproc = 1; nproc < size; nproc++) {
      MPI_Send(&discretization, 1, MPI_DOUBLE, nproc, 0, MPI_COMM_WORLD);
      MPI_Send(&initialInterval, 1, MPI_DOUBLE, nproc, 0, MPI_COMM_WORLD);
      MPI_Send(&subintervalSize, 1, MPI_DOUBLE, nproc, 0, MPI_COMM_WORLD);

      initialInterval += subintervalSize;
    }

    total = 0;
    for (nproc = 1; nproc < size; nproc++) {
      MPI_Recv(
        &calculatedValue, 
        1, 
        MPI_DOUBLE, 
        MPI_ANY_SOURCE, 
        MPI_ANY_TAG, 
        MPI_COMM_WORLD, 
        &status
      );

      total += calculatedValue;
      printf("Result: %lf\n", total);
    }
  } else {
    MPI_Recv(&discretization, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&initialInterval, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&subintervalSize, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

    calculatedValue = calcInterval(initialInterval, subintervalSize, discretization);

    MPI_Send(&calculatedValue, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}

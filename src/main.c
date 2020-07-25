#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpich/mpi.h>

#define MASTER 0 // Rank 0 is the master node
#define MAX_INTERVAL 100
#define MIN_INTERVAL 0

// Colors to prompt out
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

/**
 * Calcs the subinterval specified by the master node, using the parameterized discretization value.
 * 
 * @param intervStart     initial value of the interval
 * @param intervEnd       size of subinterval to be calculated
 * @param discretization  discretization value to consider within the calc
 */
double calcInterval(int intervStart, int intervEnd, double discretization) {
  double result, point1, point2, currVal;

  result = 0;
  for (currVal = intervStart; currVal <= intervEnd - discretization; currVal += discretization) {
    double a = currVal;
    double b = a + discretization;

    point1 = sqrt(pow(100, 2) - pow(a, 2));
    point2 = sqrt(pow(100, 2) - pow(b, 2));

    result += (point1 + point2) * (discretization / 2);
  }

  return result;
}

/**
 * Validates the user's entry that represents the number of nodes to consider. As the project statement says, only 1, 2,
 *  4 or 10 slaves nodes are allowed, so this function verifies that.
 * 
 * @param numberOfNodes
 */
void validateEntry(int numberOfNodes) {
  if (numberOfNodes != 1 && numberOfNodes != 2 && numberOfNodes != 4 && numberOfNodes != 10) {
    printf("The number of slave nodes allowed is 1, 2, 4 or 10\n");
    exit(0);
  }
}

int main(int argc, char *argv[]) {
  int size, rank, nproc, subintervalSize, currIntervalStart, subintervalThreshold;
  double discretization, calculatedValue, total;
  MPI_Status status;

  // The discretization value is passed as command line argument
  discretization = strtold(argv[1], NULL);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  validateEntry(size - 1); // Excludes the master node

  // Each child node will calc a subinterval of the integral
  currIntervalStart = MIN_INTERVAL;
  subintervalSize = MAX_INTERVAL / (size - 1); // Excludes the master node

  if (rank == MASTER) {
    for (nproc = 1; nproc < size; nproc++) {
      subintervalThreshold = subintervalSize + currIntervalStart;

      printf(ANSI_COLOR_BLUE"MASTER: "ANSI_COLOR_RESET"Sending payload to slave[%d]\n\n", nproc);

      MPI_Send(&discretization, 1, MPI_DOUBLE, nproc, 0, MPI_COMM_WORLD);
      MPI_Send(&currIntervalStart, 1, MPI_INT, nproc, 0, MPI_COMM_WORLD);
      MPI_Send(&subintervalThreshold, 1, MPI_INT, nproc, 0, MPI_COMM_WORLD);

      currIntervalStart += subintervalSize;
    }

    total = 0;
    for (nproc = 1; nproc < size; nproc++) {
      MPI_Recv(&calculatedValue, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      printf(
        ANSI_COLOR_BLUE"MASTER: "ANSI_COLOR_RESET"Received %lf from slave[%d]. Adding to total value\n\n",
        calculatedValue,
        nproc
      );

      total += calculatedValue;
    }

    printf(ANSI_COLOR_CYAN"FINAL RESULT: "ANSI_COLOR_RESET"%lf\n\n", total);
  } else {
    MPI_Recv(&discretization, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&currIntervalStart, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&subintervalThreshold, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    printf(
      ANSI_COLOR_GREEN"SLAVE[%d]: "ANSI_COLOR_RESET"Payload received. Calculating from %d to %d...\n\n",
      rank,
      currIntervalStart,
      subintervalThreshold
    );

    calculatedValue = calcInterval(
      currIntervalStart,
      subintervalThreshold,
      discretization
    );

    printf(
      ANSI_COLOR_GREEN"SLAVE[%d]: "ANSI_COLOR_RESET"Calc complete. Sending %lf to master\n\n",
      rank,
      calculatedValue
    );

    MPI_Send(&calculatedValue, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}

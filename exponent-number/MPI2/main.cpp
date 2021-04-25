#include "mpi.h"
#include <iostream>


int main(int argc, char* argv[]) {
	long n = 1000;
	int rank, size;
	long double fact = 1, partialSum = 0, myExp = 0;
	double time = 0.0;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (argc != 2) {
		if (rank == 0) std::cout << "Incorrect arguments, default number " << n << " of iterations is using" << std::endl;
	} else {
		n = atol(argv[1]);
	}

	if (rank == 0) {
		time = MPI_Wtime();
	}

	for (int i = rank; i <= n; i += size) {
		for (int j = i; j > i - size && j > 0; j--) {
			fact *= j;
		}
		partialSum += 1 / fact;
	}

	MPI_Reduce(&partialSum, &myExp, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		time = MPI_Wtime() - time;
		std::cout.precision(100);
		std::cout << "Exp:\t" << myExp << std::endl;
		std::cout.precision(4);
		std::cout << "Time:\t" << time << std::endl;
	}

	MPI_Finalize();
	return 0;
}
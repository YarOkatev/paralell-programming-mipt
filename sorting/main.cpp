#include "sort.h"

const bool OUTPUT_OMITTED = true;

int main(int argc, char* argv[]) {
	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (argc != 3)
	{
		std::cout << "Usage: ./sorting seed arraySize" << std::endl;
		return 1;
	}
	int seed = atoi(argv[1]);
	int arraySize = atoi(argv[2]);

	std::vector<int> array(arraySize);
	if (seed > 0) {
		generateRandomArray(array, seed);
	}
	else {
		generateSortedArray(array);
	}

	if (rank == 0 && !OUTPUT_OMITTED) {
		std::cout << "Initial size: " << array.size() << std::endl;
		std::cout << "Initial array: " << std::endl;
		printVector(array);
	}

	double time = MPI_Wtime();
	shellSort(array);
	time = MPI_Wtime() - time;

	if (rank == 0 && !OUTPUT_OMITTED) {
		std::cout << "Time: " << time << std::endl;
		std::cout << "Shell sort: Done" << std::endl;
		printVector(array);
	} else if (rank == 0) {
		std::cout << time << std::endl;
	}

	MPI_Finalize();
	return 0;
}

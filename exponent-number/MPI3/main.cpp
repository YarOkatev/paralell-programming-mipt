#include "mpi.h"
#include "LongDouble.h"


int main(int argc, char* argv[]) {
	long n = 100;
	int rank, size;
	LongDouble fact (1.0);
	LongDouble partialSum (0.0);
	LongDouble myExp (0.0);
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
			fact *= LongDouble(j);
		}
		partialSum += LongDouble(1) / fact;
	}

	if (rank != 0) {
		partialSum.getRawData()->push_back(reinterpret_cast<int*>(partialSum.getRawExponent())[0]);
		partialSum.getRawData()->push_back(reinterpret_cast<int*>(partialSum.getRawExponent())[1]);
		MPI_Send(partialSum.getRawData()->data(), partialSum.getRawData()->size(), MPI_INT, 0, 5, MPI_COMM_WORLD);
	}

	MPI_Status status, s;
	LongDouble buf;
	int bufSize = 0;
	if (rank == 0) {
		for (int i = 0; i < size - 1; i++) {
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_INT, &bufSize);
			buf.getRawData()->resize(bufSize);
			MPI_Recv(buf.getRawData()->data(), bufSize, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
			reinterpret_cast<int*>(buf.getRawExponent())[1] = buf.getRawData()->back();
			buf.getRawData()->pop_back();
			reinterpret_cast<int*>(buf.getRawExponent())[0] = buf.getRawData()->back();
			buf.getRawData()->pop_back();
			partialSum += buf;
			buf.getRawData()->clear();
		}
	}

	if (rank == 0) {
		time = MPI_Wtime() - time;
		std::cout << "Exp:\t" << partialSum << std::endl;
		std::cout << "Time:\t" << time << std::endl;
	}

	MPI_Finalize();
	return 0;
}
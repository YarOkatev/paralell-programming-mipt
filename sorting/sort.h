//
// Created by Ярослав Окатьев  on 15.04.2021.
//

#ifndef SORTING_SORT_H
#define SORTING_SORT_H

#include <iostream>
#include <mpi.h>
#include <vector>

#define LOG(X) ( std::cout << X << std::endl )

void printVector(const std::vector<int> &array);
void shellSort(std::vector<int> &array);
void insertionSort(std::vector<int> &array);
void generateRandomArray(std::vector<int> &array, int seed);
void generateSortedArray(std::vector<int> &array);

void generateRandomArray(std::vector<int> &array, int seed) {
	std::srand(seed);
	for (auto &x : array) {
		x = std::rand();
	}
}

void generateSortedArray(std::vector<int> &array) {
	int n = 1;
	for (auto &x : array) {
		x = n++;
	}
}

void printVector(const std::vector<int> &array) {
	for (int x : array)
		std::cout << x << ' ';
	std::cout << std::endl;
}


void shellSort(std::vector<int> &array) {
	int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	std::vector<int> procArray;

	int twoInPower = 1;
	int i, j, k;
	for (i = 0; i <= log2(size); i++) {
		twoInPower = pow(2, i);
		procArray.resize(0);
		for (j = 0; j < array.size() / size * twoInPower; j++) {
			procArray.push_back(array[rank + static_cast<size_t>(size / twoInPower * j)]);
		}
		insertionSort(procArray);

		if (rank != 0) {
			MPI_Send(procArray.data(), procArray.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
		}

		if (rank == 0) {
			for (k = 0; k < array.size() / size * twoInPower; k++) {
				array[rank + static_cast<size_t>(size / twoInPower * k)] = procArray[k];
			}

			for (j = 1; j < size / twoInPower; j++) {
				MPI_Status status;
				MPI_Recv(procArray.data(), static_cast<size_t>(array.size() / twoInPower), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
				for (k = 0; k < array.size() / size * twoInPower; k++) {
					array[status.MPI_SOURCE + static_cast<size_t>(size / twoInPower * k)] = procArray[k];
				}
			}
		}

		if (rank >= pow(2, log2(size) - i - 1)) {
			return;
		}
	}
}


void insertionSort(std::vector<int> &array) {
	for (int i = 1; i < array.size(); i++) {
		for (int j = i; j > 0 && array[j - 1] > array[j]; j--) {
			std::swap(array[j - 1], array[j]);
		}
	}
}

#endif //SORTING_SORT_H
//
// Created by Ярослав Окатьев  on 25.04.2021.
//

#ifndef DIFFUSION_EQUATION_SOLVER_H
#define DIFFUSION_EQUATION_SOLVER_H

#include <iostream>
#include "mpi.h"

const int X_STEPS = 100;
const double X = 1;
const int TIME_STEPS = 100;
const double T = 1;

inline double zeroCoordConditions(double t);
inline double rightPart(double t, double x);
inline double zeroTimeConditions(double x);
void printSolution(double** u);
void clearMemory(double** u);
int parallel();
int sequential();

double rightPart(double t, double x) {
	return 10 * x * t;
}

double zeroCoordConditions(double t) {
	return 10 * t;
}

double zeroTimeConditions(double x) {
	return 10 * sin(10 * x);
}

void clearMemory(double** u) {
	for (int i = 0; i < TIME_STEPS; i++) {
		delete[] u[i];
	}
	delete[] u;
}

void printSolution(double** u) {
	double h = X / X_STEPS;
	double t = T / TIME_STEPS;

	std::cout.precision(10);
	for (int k = 0; k < TIME_STEPS; k++) {
		for (int m = 0; m < X_STEPS; m++) {
			std::cout << std::fixed << k * t << '\t';
			std::cout << std::fixed << m * h << '\t';
			std::cout << std::fixed << u[k][m] << '\n';
		}
	}
}

int parallel() {
	int rank, size;

	MPI_Init(nullptr, nullptr);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double h = X / X_STEPS;
	double t = T / TIME_STEPS;

	auto u = new double* [TIME_STEPS];
	for (int i = 0; i < TIME_STEPS; i++) {
		u[i] = new double[X_STEPS];
	}

	for (int i = 0; i < X_STEPS; i++) {
		u[0][i] = zeroTimeConditions(i * h);
	}
	for (int i = 0; i < TIME_STEPS; i++) {
		u[i][0] = zeroCoordConditions(i * t);
	}

	MPI_Status status;
	MPI_Request request;
	for (int k = rank; k < TIME_STEPS - 1; k += size) {
		for (int m = 1; m < X_STEPS; m++) {
			if (k != 0) {
				MPI_Recv(&u[k][m], 1, MPI_DOUBLE, (k - 1) % size, 0, MPI_COMM_WORLD, &status);
			}

			u[k + 1][m] = 1.0 / (1.0 / (2 * h) + 1.0 / (2 * t)) *
						(rightPart(k * t + 1.0 / 2 * t, m * h - 1.0 / 2 * h) +
						 1.0 / (2 * t) * (u[k][m] + u[k][m - 1] - u[k + 1][m - 1]) +
						 1.0 / (2 * h) * (u[k + 1][m - 1] - u[k][m] + u[k][m - 1]));

			if (k != TIME_STEPS - 2) {
				MPI_Isend(&u[k + 1][m], 1, MPI_DOUBLE, (rank + 1) % size, 0, MPI_COMM_WORLD, &request);
			}
		}
	}

	for (int k = 2; k < TIME_STEPS; k++) {
		if (k % size != 0 && k % size != 1) {
			if (rank == k % size - 1) {
				MPI_Send(u[k], X_STEPS, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
			}
			if (rank == 0) {
				MPI_Recv(u[k], X_STEPS, MPI_DOUBLE, k % size - 1, 0, MPI_COMM_WORLD, &status);
			}
		}
	}

	if (TIME_STEPS % size == 1) {
		if (rank == size - 1) {
			MPI_Send(u[TIME_STEPS - 1], X_STEPS, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		}
		if (rank == 0) {
			MPI_Recv(u[TIME_STEPS - 1], X_STEPS, MPI_DOUBLE, size - 1, 0, MPI_COMM_WORLD, &status);
		}
	}

	if (rank == 0) {
		printSolution(u);
	}
	clearMemory(u);

	MPI_Finalize();
	return 0;
}

int sequential() {
	double h = X / X_STEPS;
	double t = T / TIME_STEPS;

	auto u = new double* [TIME_STEPS];
	for (int i = 0; i < TIME_STEPS; i++) {
		u[i] = new double[X_STEPS];
	}

	for (int i = 0; i < X_STEPS; i++) {
		u[0][i] = zeroTimeConditions(i * h);
	}
	for (int i = 0; i < TIME_STEPS; i++) {
		u[i][0] = zeroCoordConditions(i * t);
	}

	for (int k = 0; k < TIME_STEPS - 1; k++) {
		for (int m = 1; m < X_STEPS; m++) {
			u[k + 1][m] = 1.0 / (1.0 / (2 * h) + 1.0 / (2 * t)) *
						  (rightPart(k * t + 1.0 / 2 * t, m * h - 1.0 / 2 * h) +
						   1.0 / (2 * t) * (u[k][m] + u[k][m - 1] - u[k + 1][m - 1]) +
						   1.0 / (2 * h) * (u[k + 1][m - 1] - u[k][m] + u[k][m - 1]));
		}
	}

	printSolution(u);
	clearMemory(u);

	return 0;
}

#endif //DIFFUSION_EQUATION_SOLVER_H

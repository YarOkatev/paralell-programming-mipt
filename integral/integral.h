//
// Created by Ярослав Окатьев  on 27.04.2021.
//

#ifndef INTEGRAL_INTEGRAL_H
#define INTEGRAL_INTEGRAL_H

#include <pthread.h>
#include <cmath>
#include <iostream>
#include <vector>

#define LOG(X) (std::cout << X << std::endl);
const int NUM_THREADS = 10;
const double PI = acos(-1.0);

typedef struct {
	double leftBound;
	double rightBound;
} thread_data_t;

void* thrIntegrate(void* arg);
double intFunc(double x); // function for integrating
double getNextZero(); // returns zeros of function intFunc between limits from greatest to least


double integrate(double leftLim, double rightLim) {
	pthread_t thrIds[NUM_THREADS];
	std::vector<thread_data_t> thrData[NUM_THREADS];

	thrData[0].push_back({getNextZero(), rightLim});
	for (int i = 1 ; ; i++) {
		if (thrData[i % NUM_THREADS].capacity() == thrData[i % NUM_THREADS].size()) {
			thrData[i % NUM_THREADS].reserve(thrData[i % NUM_THREADS].size() + 100);
		}

		thrData[i % NUM_THREADS].push_back({getNextZero(), (thrData[(i - 1) % NUM_THREADS].end() - 1)->leftBound});
		if ((thrData[i % NUM_THREADS].end() - 1)->leftBound < leftLim) {
			(thrData[i % NUM_THREADS].end() - 1)->leftBound = leftLim;
			break;
		}
	}

	int rc;
	for (int i = 0; i < NUM_THREADS; ++i) {
		if ((rc = pthread_create(&thrIds[i], nullptr, thrIntegrate, &thrData[i]))) {
			std::cout << "Error with creating thread: " << rc << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	double *retBuff;
	double sum = 0;
	for (auto & x : thrIds) {
		if ((rc = pthread_join(x, reinterpret_cast<void**>(&retBuff)))) {
			std::cout << "Error with joining thread: " << rc << std::endl;
			exit(EXIT_FAILURE);
		}
		sum += *retBuff;
		delete retBuff;
	}

	return sum;
}

void* thrIntegrate(void* arg) {
	auto data = reinterpret_cast<std::vector<thread_data_t>*>(arg);
	double n = 1000000;
	auto subSum = new double(0);
	double h = 0;

	for (auto it = data->begin(); it != data->end(); it++) {
		h = (it->rightBound - it->leftBound) / n;
		for (int i = 0; i <= n; i++) {
			*subSum += h * intFunc(it->rightBound + i * h + h / 2.0);
		}
	}
	LOG(*subSum)
	return reinterpret_cast<void*>(subSum);
}


double intFunc(double x) {
	return sin(1.0 / x);
}

double getNextZero() {
	static int callNum = 0;
	callNum++;
	return 1.0 / (PI * callNum);
}

#endif //INTEGRAL_INTEGRAL_H

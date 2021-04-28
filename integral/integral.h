//
// Created by Ярослав Окатьев  on 27.04.2021.
//

#ifndef INTEGRAL_INTEGRAL_H
#define INTEGRAL_INTEGRAL_H

#include <pthread.h>
#include <cmath>
#include <iostream>
#include <vector>

#define LOG(X) (std::cout << X << '\n');

const double PI = acos(-1.0);

typedef struct {
	double leftBound;
	double rightBound;
} IntBounds;

typedef struct {
	std::vector<IntBounds>* bounds;
	unsigned long long n;
} ThrData;

void* thrIntegrate(void* arg);
double intFunc(double x); // function for integrating
double getNextZero(); // returns zeros of function intFunc between limits from greatest to least
unsigned long long int stepsForPrecision(double prec);
double integrate(double leftLim, double rightLim, double prec, unsigned int threadNum);

double integrate(double leftLim, double rightLim, double prec, unsigned int threadNum) {
	pthread_t thrIds[threadNum];
	std::vector<IntBounds> thrBounds[threadNum];
	ThrData thrData[threadNum];

	thrBounds[0].push_back({getNextZero(), rightLim});
	for (int i = 1;; i++) {
		if (thrBounds[i % threadNum].capacity() == thrBounds[i % threadNum].size()) {
			thrBounds[i % threadNum].reserve(thrBounds[i % threadNum].size() + 100);
		}

		thrBounds[i % threadNum].push_back({getNextZero(), (thrBounds[(i - 1) % threadNum].end() - 1)->leftBound});
		if ((thrBounds[i % threadNum].end() - 1)->leftBound < leftLim) {
			(thrBounds[i % threadNum].end() - 1)->leftBound = leftLim;
			break;
		}
	}

	int rc;
	unsigned long long steps = stepsForPrecision(prec);
	for (int i = 0; i < threadNum; ++i) {
		thrData[i] = {&thrBounds[i], steps};
		if ((rc = pthread_create(&thrIds[i], nullptr, thrIntegrate, &thrData[i]))) {
			std::cout << "Error with creating thread: " << rc << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	double* retBuff;
	double sum = 0;
	for (auto &x : thrIds) {
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
	auto data = reinterpret_cast<ThrData*>(arg);
	auto subSum = new double(0);
	double h = 0;

	for (auto it = data->bounds->begin(); it != data->bounds->end(); it++) {
		h = (it->rightBound - it->leftBound) / data->n;
		for (unsigned long long i = 0; i < data->n; i++) {
			*subSum += h * intFunc(it->leftBound + i * h + h * 0.5);
		}
	}

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

unsigned long long int stepsForPrecision(double prec) {
	return static_cast<unsigned long long>(sqrt(1.69 / prec) + 1); // 1.69 is upper bound for |f''| * (b - a) / 24
}


#endif //INTEGRAL_INTEGRAL_H

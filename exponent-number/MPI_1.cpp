#include <iostream>


int main(int argc, char* argv[]) {
	long n = 1000;
	long double myExp = 1.0;
	long double fact = 1.0;

	if (argc != 2) {
		std::cout << "Incorrect arguments, default number " << n << " of iterations is using" << std::endl;
	} else {
		n = atol(argv[1]);
	}

	for (int i = 1; i < n; i++) {
		fact *= i;
		myExp += 1 / fact;
	}

	std::cout.precision(100);
	std::cout << "Exp:\t" << myExp << std::endl;

	return 0;
}

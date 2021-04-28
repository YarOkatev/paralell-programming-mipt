#include "integral.h"

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "Usage: ./exename precision threadNum\n";
	} else {
		double prec = std::strtod(argv[1], nullptr);
		unsigned int threadNum = std::strtoul(argv[2], nullptr, 10);
		std::cout.precision(ceil(-log10(prec)));
		std::cout << "Result: " << integrate(0.001, 1, prec, threadNum) << std::endl;
	}
	return 0;
}

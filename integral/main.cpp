#include "integral.h"

int main() {
	std::cout.precision(10);
	std::cout << "Int: " << integrate(0.001, 1) << std::endl;
	return 0;
}

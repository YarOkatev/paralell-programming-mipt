#include "solver.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cout << "Usage: exe_name p / s" << std::endl;
		return -1;
	}

	if (argv[1][0] == 's') {
		sequential();
	} else {
		parallel();
	}
	return 0;
}
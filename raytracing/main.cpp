#include "rtx.h"
#include "mpi.h"


int main(int argc, char* argv[]) {
	const int height = 1000;
	const int width = 1400;
	unsigned char data[height * width];
	bzero(data, height * width);
	const float fov = 3.14159 / 1.7;
	float x, y;
	double time;

	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	std::vector<Triangle> trs;
	readTrianglesFromFile("../model.xyz", trs, 10000);

	if (rank == 0) {
		std::cout << "Cluster size: " << size << std::endl;
		std::cout << "Triangles amount: " << trs.size() << std::endl;
		time = MPI_Wtime();
	}

	Vec3f dir, cameraPos(10, 1, -6), lightSource(5, -5, 0), rotation(-2, 0, 1);
	float brightness = 0;

	for (size_t j = height / size * rank; j < height / size * (rank + 1); j++) {
		for (size_t i = 0; i < width; i++) {
			x = (2 * (i + 0.5) / (float)width - 1) * tan(fov / 2.) * width / (float)height;
			y = -(2 * (j + 0.5) / (float)height - 1) * tan(fov / 2.);
			dir = (Vec3f(x, y, 1) + rotation).normalize();

			brightness = LIGHT_BASE * (castRay(cameraPos, dir, lightSource, trs, 1));
			brightness = brightness > 255 ? (char)255 : (char)brightness;
			data[i + j * width] = brightness;
		}
	}

	MPI_Gather(&data[height / size * rank * width], height * width / size, MPI_CHAR, data, height * width / size, MPI_CHAR, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		std::cout << "Time: " << MPI_Wtime() - time << std::endl;
		char pic[] = "../result.jpg";
		saveImage(pic, width, height, data);
	}

	MPI_Finalize();

	return 0;
}

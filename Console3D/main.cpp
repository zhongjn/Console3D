//#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "console.h"
#include "core.h"
#include "present.h"
#include <math.h>
#include <chrono>

using namespace console3d;
using namespace core;

const int width = 320, height = 240;


Mesh cube() {
	int multi = 30;

	Vertex temp;
	VertexTriangle triangle;

	std::vector<Vertex> vertexes;
	std::vector<VertexTriangle> triangles;

	int current = 0;
	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			int t = (i + j + 2) * 60;
			temp.position = Vector<3>(1, i, j);
			temp.color = Color(255, (i + j + 2) * multi, (i + j + 2) * multi);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(-1, i, j);
			temp.color = Color(255, (i + j + 2) * multi, (i + j + 2) * multi);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(i, 1, j);
			temp.color = Color((i + j + 2) * multi, 255, (i + j + 2) * multi);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(i, -1, j);
			temp.color = Color((i + j + 2) * multi, 255, (i + j + 2) * multi);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(i, j, 1);
			temp.color = Color((i + j + 2) * multi, (i + j + 2) * multi, 255);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(i, j, -1);
			temp.color = Color((i + j + 2) * multi, (i + j + 2) * multi, 255);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;


	return Mesh(vertexes, triangles);

}


int main(int argc, char **argv) {

	system("cls");

	Vector<3> vec(1, 2, 3);
	int a = 1 + 2;

	Core3DContext context(width, height);

	float angle = 0;

	//console::initialize(size, size);
	present::initialize(GetConsoleWindow(), width, height, 1);

	auto cam_trans =
		Transformation().translate(Vector<3>(0.0, 0.0, -10.0)).rotate(X, 0.7);
	Camera camera(cam_trans, Size{ (float)width / (float)height, 1.0 });

	context.camera = camera;


	auto cube1 = cube();
	cube1.transformation = Transformation().translate(Position(-2, 0 ,-2));

	auto cube2 = cube();
	cube2.transformation = Transformation().translate(Position(-2, 0, 2));

	auto cube3 = cube();
	cube3.transformation = Transformation().translate(Position(2, 0, -2));

	auto cube4 = cube();
	cube4.transformation = Transformation().translate(Position(2, 0, 2));



	int frames = 0;
	auto prev = std::chrono::high_resolution_clock::now();

	while (true) {
		if (frames == 100) {
			auto now = std::chrono::high_resolution_clock::now();
			auto d = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count();
			prev = now;
			frames = 0;
			printf("FPS=%f\n", 100.0 / (d / 1000.0));
		}

		angle += 0.005f;
		context.transformation_world = Transformation().rotate(Y, angle);
		context.scene_begin();


		int multi = 30;


		context.draw_mesh(cube1);
		context.draw_mesh(cube2);
		context.draw_mesh(cube3);
		context.draw_mesh(cube4);

		context.scene_end();


		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				Color color = context.pixels[j * width + i].color;
				present::set_pixel(i, j, color);
				//char ch = ' ';
				//if (color[0] >= 127 && color[1] >= 127 && color[2] >= 127) {
				//	ch = '*';
				//}
				//console::set_char(i, j, ch);
			}
		}
		//present::set_pixel(10, 10, Color(200, 200, 100));
		present::present();
		frames++;
	}
	system("pause");
	return EXIT_SUCCESS;
}
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


Mesh cube(Position pos) {
	int multi = 0;

	Vertex temp;
	Triangle triangle;

	std::vector<Vertex> vertexes;
	std::vector<Triangle> triangles;

	int current = 0;
	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			int t = (i + j + 2) * 60;
			temp.position = Vector<3>(1, i, j);
			temp.color = Color(255, 0, 0);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(-1, i, j);
			temp.color = Color(255, 0, 0);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(i, 1, j);
			temp.color = Color(255, 0, 0);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(i, -1, j);
			temp.color = Color(255, 0, 0);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(i, j, 1);
			temp.color = Color(255, 0, 0);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			temp.position = Vector<3>(i, j, -1);
			temp.color = Color(255, 0, 0);
			vertexes.emplace_back(temp);
		}
	}
	triangles.emplace_back(current, current + 1, current + 2);
	triangles.emplace_back(current + 1, current + 2, current + 3);
	current += 4;

	Mesh mesh(vertexes, triangles);
	mesh.transformation = Transformation().translate(pos);

	return mesh;

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
		Transformation().translate(Vector<3>(0.0, 0, -20.0)).rotate(X, 0.5);
	Camera camera(cam_trans, Size{ (float)width / (float)height, 1.0 });

	context.camera = camera;

	const int from = -1;
	const int to = 1;
	const int t = to - from + 1;

	Mesh cubes[t*t*t];
	int index = 0;
	for (int i = from; i <= to; i++) {
		for (int j = from; j <= to; j++) {
			for (int k = from; k <= to; k++) {
				cubes[index] = cube(Position(i, j, k) * 4);
				index++;
			}
		}
	}

	AmbientLight ambient_light;
	ambient_light.intensity = 0.0;

	std::vector<PointLight> point_light;

	PointLight p1;
	p1.position = Position(1.5, 0, -1.5);
	p1.intensity = 5;
	point_light.emplace_back(p1);

	int frames = 0;
	auto prev = std::chrono::high_resolution_clock::now();

	while (true) {
		if (frames == 100) {

			auto now = std::chrono::high_resolution_clock::now();
			auto d = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count();
			prev = now;
			frames = 0;
			//SetConsoleTitleA
			char title[50] = { 0 };
			sprintf_s(title, "Console3D FPS=%.2f", (float)(100.0 / (d / 1000.0)));
			SetConsoleTitle(title);
		}

		angle += 0.005f;
		//context.transformation_world = Transformation().rotate(Y, angle);
		context.scene_begin();

		context.set_ambient_light(ambient_light);
		context.set_point_light(point_light);

		int multi = 30;

		

		for (int i = 0; i < t*t*t; i++) {
			cubes[i].transformation = cubes[i].transformation.rotate(Y, 0.01f);
			context.draw_mesh(cubes[i]);
		}


		context.scene_end();


		//for (int i = 0; i < width; i++) {
		//	for (int j = 0; j < height; j++) {
		//		Color color = context.get_pixel(i, j).color;
		//		present::set_pixel(i, j, color);
		//		//char ch = ' ';
		//		//if (color[0] >= 127 && color[1] >= 127 && color[2] >= 127) {
		//		//	ch = '*';
		//		//}
		//		//console::set_char(i, j, ch);
		//	}
		//}
		present::set_all_pixels(context);
		//present::set_pixel(10, 10, Color(200, 200, 100));
		present::present();
		frames++;
	}
	system("pause");
	return EXIT_SUCCESS;
}
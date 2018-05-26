#include <windows.h>
#include "my3d.h"
#include "my3dpresent.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>

using namespace my3d;
const int width = 320, height = 240;

const float cam_height = 3.6f;
const float cam_pitch = 0.6f;
const float cam_back = -4.0f;

const int map_size = 20;

const float block_dist = 1.6f;
const float block_size = 1.0f;
const Color block_color = Color(120, 120, 120);

const int start_i = 0, start_j = 3;

enum direction {
	i_p = 0, // i正方向
	j_p = 1, // j正方向
	i_n = 2,
	j_n = 3
};

// i,j=循环变量  ii,ij=格子索引  pi,pj=位置，分别对应z轴和x轴

Mesh get_square(int ii, int ij) {
	Mesh out;
	out.transformation = Transformation().translate(Position(ij, 0, ii) * block_dist);
	std::vector<Vertex> vertexes;
	std::vector<Triangle> triangles;
	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			Vertex v;
			v.color = block_color;
			v.position = Position(j, 0, i) / 2 * block_size;
			vertexes.emplace_back(v);
		}
	}
	triangles.emplace_back(0, 1, 2);
	triangles.emplace_back(1, 2, 3);

	out.triangles = triangles;
	out.vertexes = vertexes;
	out.accept_light = true;

	return out;
}

Mesh get_snake_head() {
	std::vector<Vertex> vertexes;
	std::vector<Triangle> triangles;

	Vertex v;

	v.color = { 255, 0, 0 };

	v.position = { 0, 0, 0 };
	vertexes.emplace_back(v);

	v.position = { 0.866025, 0.000000 ,-0.500000 };
	vertexes.emplace_back(v);

	v.position = { -0.000000 ,0.000000, 1.000000 };
	vertexes.emplace_back(v);

	v.position = { -0.866025 ,0.000000, -0.500000 };
	vertexes.emplace_back(v);

	v.position = { 0.866025 ,0.500000, -0.500000 };
	vertexes.emplace_back(v);

	v.position = { -0.000000 ,0.500000 ,1.000000 };
	vertexes.emplace_back(v);

	v.position = { -0.866025, 0.500000, -0.500000 };
	vertexes.emplace_back(v);


	triangles.emplace_back(1, 2, 3);
	triangles.emplace_back(2, 1, 4);
	triangles.emplace_back(2, 4, 5);
	triangles.emplace_back(1, 3, 6);
	triangles.emplace_back(1, 6, 4);
	triangles.emplace_back(3, 2, 5);
	triangles.emplace_back(3, 5, 6);
	triangles.emplace_back(5, 4, 6);
	Mesh out(vertexes, triangles);
	out.accept_light = true;
	return out;
}


int main(int argc, char **argv) {
	printf("Press any key to start...");
	getchar();
	system("cls");

	float cur_pi = block_dist * start_i, cur_pj = block_dist * start_j, cur_r = 0.0f;
	direction cur_dir = direction::i_p;

	int map[map_size][map_size] = { 0 };

	Mesh block_mesh[map_size][map_size];
	for (int i = 0; i < map_size; i++) {
		for (int j = 0; j < map_size; j++) {
			block_mesh[i][j] = get_square(i, j);
		}
	}
	Mesh snake_head = get_snake_head();

	PointLight pl;
	pl.position = { 0, 3, 0 };
	pl.intensity = 3;
	pl.color = { 255, 255, 255 };

	AmbientLight al;
	al.color = { 255, 255, 255 };
	al.intensity = 0.03;


	Context context(width, height);

	present::initialize(GetConsoleWindow(), width, height, 1);

	Camera camera;
	camera.lens_size = { (float)width / (float)height, 1.0 };

	int frames = 0;
	auto prev = std::chrono::high_resolution_clock::now();

	while (true) {
		cur_pi += 0.02f;

#pragma region FPS
		frames++;
		if (frames == 100) {

			auto now = std::chrono::high_resolution_clock::now();
			auto d = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count();
			prev = now;
			frames = 0;
			char title[50] = { 0 };
			sprintf_s(title, "Console3D FPS=%.2f", (float)(100.0 / (d / 1000.0)));
			SetConsoleTitle(title);
		}
#pragma endregion

		
		camera.transformation = Transformation().rotate(X, cam_pitch).translate(0, cam_height, 0).rotate(Y, cur_r).translate(0, 0, cam_back);

		context.scene_begin(camera);

		context.set_ambient_light(al);
		context.set_point_light(pl);

		context.set_world_transformation(Transformation().translate(-cur_pj, 0, -cur_pi));
		for (int i = 0; i < map_size; i++) {
			for (int j = 0; j < map_size; j++) {
				context.draw_mesh(block_mesh[i][j]);
			}
		}
		context.set_world_transformation(Transformation());

		snake_head.transformation = Transformation().scale(0.5).rotate(Y, cur_dir * M_PI / 2);
		context.draw_mesh(snake_head);

		context.scene_end();

		present::set_all_pixels(context);
		present::present();
		
	}
	return EXIT_SUCCESS;
}
#include <windows.h>
#include "my3d.h"
#include "my3dpresent.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include <conio.h>
#include <list>

using namespace my3d;
const int width = 320, height = 240;

const float cam_height = 7.2f;
const float cam_pitch = 0.65f;
const float cam_back = 8.0f;

const int map_size = 40;

const float block_dist = 1.6f;
const float block_size = 1.0f;
const Color block_color = Color(120, 120, 120);

const float snake_length = 8.0f;
const float snake_width = 0.6f;

const int steps_per_block = 20;
const float step = block_dist / steps_per_block;

const int start_i = 20, start_j = 20;

enum direction {
	i_p = 0, // i正方向
	j_p = 1, // j正方向
	i_n = 2,
	j_n = 3
};

template<typename T>
struct tuple {
	T i, j;
};

struct turn_info {
	tuple<int> block;
	direction origin_dir;
};

Mesh get_square(int ii, int ij);
Mesh get_snake_head();
Mesh get_snake_body(float length, tuple<float> pos, direction dir);

tuple<float> block_to_pos(tuple<int> block);
direction dir_reverse(direction dir);
float dir_rotation(direction dir);

template<typename T>
tuple<T> apply_dir_delta(tuple<T> pos, direction dir, T delta) {
	switch (dir) {
	case direction::i_p:
		pos.i += delta; break;
	case direction::i_n:
		pos.i -= delta; break;
	case direction::j_p:
		pos.j += delta; break;
	case direction::j_n:
		pos.j -= delta; break;
	}
	return pos;
}

int main(int argc, char **argv) {
	//printf("Press any key to start...");
	//getchar();
	system("cls");

	std::vector<turn_info> turns;
	tuple<int> cur_block = { start_i, start_j };
	direction cur_dir = direction::i_p;

	float dr = 0;

	int nstep = 0;

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

	present::initialize(GetConsoleWindow(), width, height, 2);

	Camera camera;
	camera.lens_size = { (float)width / (float)height, 1.0 };

	int frames = 0;
	auto prev = std::chrono::high_resolution_clock::now();



	while (true) {

		tuple<float> cur_pos = { cur_block.i * block_dist, cur_block.j * block_dist };
		cur_pos = apply_dir_delta(cur_pos, cur_dir, nstep * step);

		float cur_r = float(cur_dir) * M_PI / 2;
		float smooth_r = dr + cur_r;

#pragma region FPS
		frames++;
		if (frames == 100) {

			auto now = std::chrono::high_resolution_clock::now();
			auto d = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count();
			prev = now;
			frames = 0;
			char title[50] = { 0 };
			sprintf_s(title, "SnakeDemo FPS=%.2f", (float)(100.0 / (d / 1000.0)));
			SetConsoleTitle(title);
		}
#pragma endregion


		camera.transformation = Transformation().rotate(X, cam_pitch).translate(0, 0, -cam_back).rotate(Y, smooth_r).translate(0, cam_height, 0);

		context.scene_begin(camera);

		context.set_ambient_light(al);
		context.set_point_light(pl);

		context.set_world_transformation(Transformation().translate(-cur_pos.j, 0, -cur_pos.i));
		for (int i = 0; i < map_size; i++) {
			for (int j = 0; j < map_size; j++) {
				context.draw_mesh(block_mesh[i][j]);
			}
		}

		//0.65
		snake_head.transformation = Transformation().scale(0.5).translate(0, 0, 0.25).rotate(Y, cur_r).translate(cur_pos.j, 0, cur_pos.i);
		context.draw_mesh(snake_head);

		float len_remain = snake_length;
		turn_info turn_prev = { cur_block, cur_dir };
		auto pos_prev = cur_pos;

		tuple<float> last_pos;
		direction last_dir;

		int truncate_size = 0;
		if (turns.size() > 0) {
			for (auto turn : turns) {
				truncate_size++;
				auto pos = block_to_pos(turn.block);
				float len = fabsf(pos_prev.i - pos.i + pos_prev.j - pos.j);
				if (len >= len_remain) {
					last_dir = turn_prev.origin_dir;
					last_pos = pos_prev;
					break;
				}
				len_remain -= len;
				Mesh body = get_snake_body(len + snake_width / 2, pos_prev, turn_prev.origin_dir);
				context.draw_mesh(body);
				turn_prev = turn;
				pos_prev = pos;
			}
		}
		else {
			last_pos = cur_pos;
			last_dir = cur_dir;
		}
		Mesh last = get_snake_body(len_remain, last_pos, last_dir);
		context.draw_mesh(last);

		//tail here

		turns.resize(truncate_size);

		context.set_world_transformation(Transformation());


		context.scene_end();

		present::set_all_pixels(context);
		present::present();

		nstep++;
		if (nstep >= steps_per_block) {
			nstep = 0;

			cur_block = apply_dir_delta(cur_block, cur_dir, 1);

			
			int turn = 0; // -1=right 1=left 

			if (_kbhit()) {
				int ch = _getch();
				if (ch == 224) {
					int ch2 = _getch();
					if (ch2 == 75) {
						turn = 1;
					}
					else if (ch2 == 77) {
						turn = -1;
					}
				}
			}
			if (turn != 0) {
				turns.insert(turns.begin(), turn_info{ cur_block, cur_dir });
			}
			
			cur_dir = direction((int(cur_dir) + turn + 4) % 4); // turn left
			dr = -turn * M_PI / 2;
		}
		dr *= 0.8;

	}
	return EXIT_SUCCESS;
}


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

Mesh get_snake_body(float length, tuple<float> pos, direction dir) {
	std::vector<Vertex> vs;
	Vertex v;
	v.color = { 255, 0, 0 };
	vs.emplace_back(v);
	float w2 = snake_width / 2;
	v.position = { -w2,0.000000, length }; vs.emplace_back(v);
	v.position = { -w2,0.000000,0.000000 }; vs.emplace_back(v);
	v.position = { w2,0.000000,0.000000 }; vs.emplace_back(v);
	v.position = { w2,0.000000, length }; vs.emplace_back(v);
	v.position = { -w2,0.250000, length }; vs.emplace_back(v);
	v.position = { -w2,0.250000,0.000000 }; vs.emplace_back(v);
	v.position = { w2,0.250000, length }; vs.emplace_back(v);
	v.position = { w2,0.250000,0.000000 }; vs.emplace_back(v);

	std::vector<Triangle> ts;
	ts.emplace_back(1, 2, 3);
	ts.emplace_back(1, 3, 4);
	ts.emplace_back(2, 1, 5);
	ts.emplace_back(2, 5, 6);
	ts.emplace_back(1, 4, 7);
	ts.emplace_back(1, 7, 5);
	ts.emplace_back(4, 3, 8);
	ts.emplace_back(4, 8, 7);
	ts.emplace_back(3, 2, 6);
	ts.emplace_back(3, 6, 8);
	ts.emplace_back(6, 5, 7);
	ts.emplace_back(6, 7, 8);

	Mesh out;
	out.accept_light = true;
	out.triangles = ts;
	out.vertexes = vs;

	float rot = dir_rotation(dir_reverse(dir));
	out.transformation = Transformation().rotate(Y, rot).translate(pos.j, 0, pos.i);

	return out;
}


tuple<float> block_to_pos(tuple<int> block) {
	return { block.i * block_dist, block.j * block_dist };
}

direction dir_reverse(direction dir) {
	return direction((int(dir) + 2) % 4);
}

float dir_rotation(direction dir) {
	return float(dir) * M_PI / 2;
}
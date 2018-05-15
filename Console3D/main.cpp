//#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "console.h"
#include "core.h"
#include <math.h>
using namespace console3d;
using namespace core;
int main(int argc, char **argv) {


	Core3DContext context(50, 50);

	float dist = 2.5;
	float angle = 0;

	console::initialize(50, 50);

	while (true) {
		angle += 0.01;
		Camera camera(Vector3(0.5, sinf(angle) * dist, -cosf(angle) * dist ), Vector3(angle, 0, 0), Size{ 1, 1 });

		context.draw_begin(camera);

		Line line1;
		line1.position = Vector3(0, 0, 0);
		line1.orientation = Vector3(0, 0, 1);

		Line line2;
		line2.position = Vector3(0, 0, 0);
		line2.orientation = Vector3(0, 1, 0);

		Line line3;
		line3.position = Vector3(0, 0, 0);
		line3.orientation = Vector3(1, 0, 0);

		Line line4;
		line4.position = Vector3(1, 0, 0);
		line4.orientation = Vector3(-1, 1, 0);

		Line line5;
		line5.position = Vector3(0, 1, 0);
		line5.orientation = Vector3(0, -1, 1);

		Line line6;
		line6.position = Vector3(0, 0, 1);
		line6.orientation = Vector3(1, 0, -1);


		context.draw_line(line1);
		context.draw_line(line2);
		context.draw_line(line3);
		context.draw_line(line4);
		context.draw_line(line5);
		context.draw_line(line6);

		context.draw_end();


		for (int i = 0; i < 50; i++) {
			for (int j = 0; j < 50; j++) {
				Color color = context.pixels[i * 50 + j].color;
				char ch = ' ';
				if (color.r >= 127 && color.g >= 127 && color.b >= 127) {
					ch = '.';
				}
				console::set_char(i, j, ch);
			}
		}
		console::render();
		Sleep(15);
	}
	system("pause");
	return EXIT_SUCCESS;
}
//#define WIN32_LEAN_AND_MEAN
//”√”⁄≤‚ ‘
#include <windows.h>
#include "console.h"
#include "core.h"
#include <math.h>
using namespace console3d;
using namespace core;

const int size = 30;

int main(int argc, char **argv) {


	Core3DContext context(size, size);

	float dist = 2.5;
	float angle = 0;

	console::initialize(size, size);


	auto cam_trans =
		Transformation().translate(Vector3(0, 0, -3)).rotate(Y, -0.5);
	Camera camera(cam_trans, Size{ 1, 1 });
	context.camera = camera;

	while (true) {
		angle += 0.05;
		
		context.world = Transformation().rotate(X, angle);
		context.draw_begin();

		Line line1;
		line1.transformation.translation = Vector3(0, 0, 0);
		line1.orientation = Vector3(0, 0, 1);

		Line line2;
		line2.transformation.translation = Vector3(0, 0, 0);
		line2.orientation = Vector3(0, 1, 0);

		Line line3;
		line3.transformation.translation = Vector3(0, 0, 0);
		line3.orientation = Vector3(1, 0, 0);

		Line line4;
		line4.transformation.translation = Vector3(1, 0, 0);
		line4.orientation = Vector3(-1, 1, 0);

		Line line5;
		line5.transformation.translation = Vector3(0, 1, 0);
		line5.orientation = Vector3(0, -1, 1);

		Line line6;
		line6.transformation.translation = Vector3(0, 0, 1);
		line6.orientation = Vector3(1, 0, -1);


		context.draw_line(line1);
		context.draw_line(line2);
		context.draw_line(line3);
		context.draw_line(line4);
		context.draw_line(line5);
		context.draw_line(line6);

		context.draw_end();


		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				Color color = context.pixels[i * size + j].color;
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
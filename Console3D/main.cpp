//#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "console.h"
#include "core.h"
#include <math.h>
using namespace console3d;
using namespace core;

const int size = 30;

int main(int argc, char **argv) {

	Vector<3> vec(1.0, 2.0, 3.0);
	int a = 1 + 2;

	Core3DContext context(size, size);

	float dist = 2.5;
	float angle = 0;

	console::initialize(size, size);


	auto cam_trans =
		Transformation().translate(Vector<3>(0.0, 0.0, -3.0)).rotate(X, 1.0);
	Camera camera(cam_trans, Size{ 1.0, 1.0 });
	context.camera = camera;

	while (true) {
		angle += 0.05f;

		context.world = Transformation().rotate(Y, angle);
		context.draw_begin();

		Line line1;
		line1.transformation.translation = Vector<3>(0.0, 0.0, 0.0);
		line1.orientation = Vector<3>(0.0, 0.0, 1.0);

		Line line2;
		line2.transformation.translation = Vector<3>(0.0, 0.0, 0.0);
		line2.orientation = Vector<3>(0.0, 1.0, 0.0);

		Line line3;
		line3.transformation.translation = Vector<3>(0.0, 0.0, 0.0);
		line3.orientation = Vector<3>(1.0, 0.0, 0.0);

		Line line4;
		line4.transformation.translation = Vector<3>(1.0, 0.0, 0.0);
		line4.orientation = Vector<3>(-1.0, 1.0, 0.0);

		Line line5;
		line5.transformation.translation = Vector<3>(0.0, 1.0, 0);
		line5.orientation = Vector<3>(0.0, -1.0, 1.0);

		Line line6;
		line6.transformation.translation = Vector<3>(0.0, 0.0, 1.0);
		line6.orientation = Vector<3>(1.0, 0.0, -1.0);


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
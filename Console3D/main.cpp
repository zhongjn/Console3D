//#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "console.h"
#include "core.h"
#include "present.h"
#include <math.h>
using namespace console3d;
using namespace core;

const int width = 160, height = 120;

int main(int argc, char **argv) {

	Vector<3> vec(1, 2, 3);
	int a = 1 + 2;

	Core3DContext context(width, height);

	float dist = 2.5;
	float angle = 0;

	//console::initialize(size, size);
	present::initialize(GetConsoleWindow(), width, height);

	auto cam_trans =
		Transformation().translate(Vector<3>(0.0, 0.0, -3.1)).rotate(X, 1.0);
	Camera camera(cam_trans, Size{ (float)width / (float)height, 1.0 });
	context.camera = camera;

	while (true) {
		angle += 0.05f;

		context.world_transformation = Transformation().rotate(Y, angle);
		context.draw_begin();

		// Lines

		//Line line1;
		//line1.transformation.translation = Vector<3>(0.0, 0.0, 0.0);
		//line1.orientation = Vector<3>(0.0, 0.0, 1.0);
		//Line line2;
		//line2.transformation.translation = Vector<3>(0.0, 0.0, 0.0);
		//line2.orientation = Vector<3>(0.0, 1.0, 0.0);
		//Line line3;
		//line3.transformation.translation = Vector<3>(0.0, 0.0, 0.0);
		//line3.orientation = Vector<3>(1.0, 0.0, 0.0);
		//Line line4;
		//line4.transformation.translation = Vector<3>(1.0, 0.0, 0.0);
		//line4.orientation = Vector<3>(-1.0, 1.0, 0.0);
		//Line line5;
		//line5.transformation.translation = Vector<3>(0.0, 1.0, 0);
		//line5.orientation = Vector<3>(0.0, -1.0, 1.0);
		//Line line6;
		//line6.transformation.translation = Vector<3>(0.0, 0.0, 1.0);
		//line6.orientation = Vector<3>(1.0, 0.0, -1.0);
		//context.draw_line(line1);
		//context.draw_line(line2);
		//context.draw_line(line3);
		//context.draw_line(line4);
		//context.draw_line(line5);
		//context.draw_line(line6);

		 //Polyhedron
		std::vector<Vertex> vertexes;
		vertexes.reserve(3);

		Vertex temp;

		temp.position = Vector<3>(0, 0, 0);
		temp.color = Color(255, 0, 0);
		vertexes.emplace_back(temp);

		temp.position = Vector<3>(1, 0, 0);
		temp.color = Color(0, 255, 0);
		vertexes.emplace_back(temp);

		temp.position = Vector<3>(0, 1, 0);
		temp.color = Color(0, 0, 255);
		vertexes.emplace_back(temp);

		Polyhedron poly = Polyhedron(vertexes);
		context.draw_polyhedron(poly);

		context.draw_end();


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
		Sleep(15);
	}
	system("pause");
	return EXIT_SUCCESS;
}
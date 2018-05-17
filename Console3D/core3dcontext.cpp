#include "core.h"
#include <cfloat>

namespace console3d {
	namespace core {
		Vector<2> perspect_correct(float zb, float z0, float z1, Vector<2> k);
		Vector<2> vec2_decompose(Vector<2> basis[2], Vector<2> vec);
		Vector<2> screen_coord_to_vec2(ScreenCoord s);

		Core3DContext::Core3DContext(short h, short w) {
			height = h;
			width = w;
			pixels = new Pixel[height * width];
		}

		void Core3DContext::draw_begin() {
			for (int i = 0; i < height*width; i++) {
				pixels[i].color = { 0, 0, 0 };
				pixels[i].depth = FLT_MAX;
			}
			//auto r = camera.rotation;
			//camera_derotation = Matrix3::from_rotation(r).inverse();
			camera_detransform = camera.transformation.inverse();
		}

		void Core3DContext::draw_end() {

		}

		void Core3DContext::draw_line(Line &line) {
			//Vector<3> orien = Matrix3::from_rotation(line.rotation) * line.orientation;
			//Vector<3> end = line.position + orien;
			Vector<3> start = line.transformation.apply(Vector<3>(), true);
			Vector<3> end = line.transformation.apply(line.orientation, true);
			ScreenCoord coord_start = project_to_screen(start);
			ScreenCoord coord_end = project_to_screen(end);

			ScreenCoord screen_dir = coord_end - coord_start;
			float len = screen_dir.norm();
			ScreenCoord step = screen_dir / len / 2;
			ScreenCoord cur = coord_start;
			for (int i = 0; i < len * 2; i++) {
				float depth = cur[2];
				if (depth >= camera.depth_minimum) {
					int x = (int)(cur[HORIZONTAL]);
					int y = (int)(cur[VERTICAL]);
					if (x >= 0 && x < width && y >= 0 && y < height) {
						Pixel &p = pixels[y * width + x];
						if (depth < p.depth) {
							p.color = Color{ 0xFF, 0xFF, 0xFF };
						}
					}
				}
				cur = cur + step;
			}
		}

		Vector<3> Core3DContext::project_to_screen(Vector<3> pos) {
			// (x,y)=ScreenCoord, x=vertical, y=horizontal, TopLeft=(0,0), BottomRight=(height,width)
			// z = depth
			Vector<3> temp;
			//pos = pos - camera.position;
			//pos = camera_derotation * pos;
			pos = world.apply(pos, true);
			pos = camera_detransform.apply(pos, true);
			temp[0] = pos[0] / pos[2] / camera.lens_size.width + 0.5f;
			temp[1] = pos[1] / pos[2] / camera.lens_size.height + 0.5f;

			ScreenCoord coord;
			coord[DEPTH] = pos[2];
			coord[HORIZONTAL] = (float)width * (1 - temp[0]);
			coord[VERTICAL] = (float)height * (1 - temp[1]);
			return coord;
		}

		Vector<2> perspect_correct(float zb, float z0, float z1, Vector<2> k)
		{
			Vector<2> vec;
			float denom = k[0] / z0 + k[1] / z1 + (1 - k[0] - k[1]) / zb;
			vec[0] = k[0] / z0 / denom;
			vec[1] = k[1] / z1 / denom;
			return vec;
		}
		Vector<2> screen_coord_to_vec2(ScreenCoord s) {
			Vector<2> ret;
			ret[0] = s[0];
			ret[1] = s[1];
			return ret;
		}
		Vector<2> vec2_decompose(Vector<2> b[2], Vector<2> vec)
		{
			float det = b[0][0] * b[1][1] - b[1][0] * b[0][1];
			Vector<2> ret;
			ret[0] = b[1][1] * vec[0] - b[0][1] * vec[1];
			ret[1] = -b[1][0] * vec[0] + b[0][0] * vec[1];
			return ret / det;
		}
	}
}

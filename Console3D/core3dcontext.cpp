#include "core.h"
#include <cfloat>

namespace console3d {
	namespace core {
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
			//Vector3 orien = Matrix3::from_rotation(line.rotation) * line.orientation;
			//Vector3 end = line.position + orien;
			Vector3 start = line.transformation.apply(Vector3(), true);
			Vector3 end = line.transformation.apply(line.orientation, true);
			Vector3 coord_start = project_to_screen_coord(start);
			Vector3 coord_end = project_to_screen_coord(end);

			Vector3 screen_dir = coord_end - coord_start;
			float len = screen_dir.norm();
			Vector3 step = screen_dir / len / 2;
			Vector3 cur = coord_start;
			for (int i = 0; i < len * 2; i++) {
				float depth = cur.a[2];
				if (depth >= camera.depth_minimum) {
					int x = (int)(cur.a[0] + 0.5);
					int y = (int)(cur.a[1] + 0.5);
					if (x >= 0 && x < height && y >= 0 && y < width) {
						Pixel &p = pixels[x * height + y];
						if (depth < p.depth) {
							p.color = Color{ 0xFF, 0xFF, 0xFF };
						}
					}
				}
				cur = cur + step;
			}
		}

		Vector3 Core3DContext::project_to_screen_coord(Vector3 pos) {
			// (x,y)=ScreenCoord, x=vertical, y=horizontal, TopLeft=(0,0), BottomRight=(height,width)
			// z = depth
			Vector3 out;
			//pos = pos - camera.position;
			//pos = camera_derotation * pos;
			pos = world.apply(pos, true);
			pos = camera_detransform.apply(pos, true);
			out.a[0] = pos.a[0] / pos.a[2] / camera.lens_size.height + 0.5f;
			out.a[1] = pos.a[1] / pos.a[2] / camera.lens_size.width + 0.5f;
			out.a[2] = pos.a[2]; // put camera on the Z axis

			out.a[0] = (float)height * (1 - out.a[0]);
			out.a[1] = (float)width * out.a[1];

			return out;
		}
	}
}

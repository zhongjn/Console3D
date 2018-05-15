#include "core.h"

namespace console3d {
	namespace core {
		Core3DContext::Core3DContext(short h, short w) {
			height = h;
			width = w;
			pixels = new Pixel[height * width];
		}

		void Core3DContext::draw_begin(Camera& camera) {

		}

		void Core3DContext::draw_end() {

		}

		void Core3DContext::draw_line(Line &line) {

		}

		static Vector3 project_to_screen(Vector3 position) {

			// x, y = screen coord
			// z = depth
		}
	}
}

#include "core.h"
namespace console3d {
	namespace core {
		Camera::Camera() {

		}
		Camera::Camera(Vector3 pos, Vector3 rot, Size lens)
		{
			position = pos;
			rotation = rot;
			lens_size = lens;
		}
	}
}
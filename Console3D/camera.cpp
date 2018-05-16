#include "core.h"
namespace console3d {
	namespace core {
		Camera::Camera() {

		}
		Camera::Camera(Transformation trans, Size lens)
		{
			transformation = trans;
			lens_size = lens;
		}
	}
}
#include "core.h"
#include <math.h>

namespace console3d {
	namespace core {

		float Vector3::norm() {
			float accum = 0.0f;
			for (int i = 0; i < 3; i++) {
				accum += powf(a[i], 2);
			}
			return sqrtf(accum);
		}
		Vector3 Vector3::operator*(float factor) {
			Vector3 v2;
			for (int i = 0; i < 3; i++) {
				v2.a[i] = a[i] * factor;
			}
			return v2;
		}
		Vector3 Vector3::operator/(float factor)
		{
			return (*this) * (1.0f / factor);
		}
		Vector3 Vector3::operator+(Vector3 vec) {
			Vector3 v2;
			for (int i = 0; i < 3; i++) {
				v2.a[i] = a[i] + vec.a[i];
			}
			return v2;
		}

		Vector3 Vector3::operator-() {
			Vector3 v2;
			for (int i = 0; i < 3; i++) {
				v2.a[i] = -a[i];
			}
			return v2;
		}

		Vector3 Vector3::operator-(Vector3 vec) {
			return (*this) + (-vec);
		}


		float Vector3::operator*(Vector3 vec) {
			return dot(vec);
		}

		float Vector3::dot(Vector3 vec) {
			float accum = 0.0f;
			for (int i = 0; i < 3; i++) {
				accum += a[i] * vec.a[i];
			}
			return accum;
		}

		Vector3 Vector3::cross(Vector3 vec) {
			// TODO:
			// cross product
			return Vector3();
		}

	}
}
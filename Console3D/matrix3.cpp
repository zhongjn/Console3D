#include "core.h"

namespace console3d {
	namespace core {

		Vector3 Matrix3::operator*(Vector3 v) {
			Vector3 v2;
			for (int i = 0; i < 3; i++) {
				for (int k = 0; k < 3; k++) {
					v2.a[i] += a[i][k] * v.a[k];
				}

			}
		}

		Matrix3 Matrix3::operator*(Matrix3 m) {
			Matrix3 m2;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					for (int k = 0; k < 3; k++) {
						m2.a[i][j] += a[i][k] * m.a[k][j];
					}
				}
			}
		}

		Matrix3 Matrix3::identity() {
			Matrix3 m;
			for (int i = 0; i < 3; i++) {
				m.a[i][i] = 1.0;
			}
		}

		Matrix3 Matrix3::from_rotation(Vector3 rotation) {

		}
		Matrix3 Matrix3::from_scale(float scale) {
			Matrix3 m;
			for (int i = 0; i < 3; i++) {
				m.a[i][i] = scale;
			}
		}

	}
}
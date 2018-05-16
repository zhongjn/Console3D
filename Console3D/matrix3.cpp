#include "core.h"
#include <math.h>

namespace console3d {
	namespace core {

		Vector3 Matrix3::operator*(Vector3 v) {
			Vector3 v2;
			for (int i = 0; i < 3; i++) {
				for (int k = 0; k < 3; k++) {
					v2.a[i] += a[i][k] * v.a[k];
				}
			}
			return v2;
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
			return m2;
		}

		Matrix3 Matrix3::operator*(float factor)
		{
			Matrix3 m2;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					m2.a[i][j] = a[i][j] * factor;
				}
			}
			return m2;
		}

		Matrix3 Matrix3::operator/(float factor)
		{
			return (*this) * (1.0f / factor);
		}

		Matrix3 Matrix3::operator+(Matrix3 m)
		{
			Matrix3 m2;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					m2.a[i][j] = a[i][j] + m.a[i][j];
				}
			}
			return m2;
		}

		Matrix3 Matrix3::inverse()
		{
			float det =
				a[0][0] * a[1][1] * a[2][2] + a[0][1] * a[1][2] * a[2][0] + a[0][2] * a[1][0] * a[2][1] -
				a[0][2] * a[1][1] * a[2][0] - a[0][1] * a[1][0] * a[2][2] - a[0][0] * a[1][2] * a[2][1];
			Matrix3 m;
			m.a[0][0] = a[1][1] * a[2][2] - a[2][1] * a[1][2];
			m.a[0][1] = a[0][2] * a[2][1] - a[0][1] * a[2][2];
			m.a[0][2] = a[0][1] * a[1][2] - a[0][2] * a[1][1];
			m.a[1][0] = a[1][2] * a[2][0] - a[1][0] * a[2][2];
			m.a[1][1] = a[0][0] * a[2][2] - a[0][2] * a[2][0];
			m.a[1][2] = a[1][0] * a[0][2] - a[0][0] * a[1][2];
			m.a[2][0] = a[1][0] * a[2][1] - a[2][0] * a[1][1];
			m.a[2][1] = a[2][0] * a[0][1] - a[0][0] * a[2][1];
			m.a[2][2] = a[0][0] * a[1][1] - a[1][0] * a[0][1];
			return m / det;
		}

		Matrix3 Matrix3::identity() {
			Matrix3 m;
			for (int i = 0; i < 3; i++) {
				m.a[i][i] = 1.0f;
			}
			return m;
		}

		Matrix3 Matrix3::from_rotation(Vector3 r) {
			//Rodrigues transform
			//https://blog.csdn.net/wu_wenhuan/article/details/52588921

			float ¦È = r.norm();

			if (¦È <= 0) {
				return Matrix3::identity();
			}

			r = r / ¦È;
			float cos = cosf(¦È), sin = sinf(¦È);

			Matrix3 m;
			m.a[0][1] = -r.a[2]; // a12 = -rz
			m.a[0][2] = r.a[1];  // a13 = ry
			m.a[1][0] = r.a[2];  // a21 = rz
			m.a[1][2] = -r.a[0]; // a23 = -rx
			m.a[2][0] = -r.a[1]; // a31 = -ry
			m.a[2][1] = r.a[0];  // a32 = rx;

			Matrix3 rrT;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					rrT.a[i][j] = r.a[i] * r.a[j];
				}
			}
			Matrix3 R = Matrix3::identity() * cos + rrT *(1 - cos) + m * sin;

			return R;
		}

		Matrix3 Matrix3::from_scale(float scale) {
			Matrix3 m;
			for (int i = 0; i < 3; i++) {
				m.a[i][i] = scale;
			}
			return m;
		}

	}
}
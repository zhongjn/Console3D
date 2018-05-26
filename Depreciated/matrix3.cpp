#include "my3d.h"
#include <math.h>

namespace console3d {
	namespace core {
		float * Matrix3::operator[](int i)
		{
			return a[i];
		}
		Vector<3> Matrix3::operator*(Vector<3> v) {
			Vector<3> v2;
			for (register int i = 0; i < 3; i++) {
				for (register int k = 0; k < 3; k++) {
					v2[i] += a[i][k] * v[k];
				}
			}
			return v2;
		}

		Matrix3 Matrix3::operator*(Matrix3 m) {
			Matrix3 m2;
			for (register int i = 0; i < 3; i++) {
				for (register int j = 0; j < 3; j++) {
					for (register int k = 0; k < 3; k++) {
						m2[i][j] += a[i][k] * m[k][j];
					}
				}
			}
			return m2;
		}

		Matrix3 Matrix3::operator*(float factor)
		{
			Matrix3 m2;
			for (register int i = 0; i < 3; i++) {
				for (register int j = 0; j < 3; j++) {
					m2[i][j] = a[i][j] * factor;
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
			for (register int i = 0; i < 3; i++) {
				for (register int j = 0; j < 3; j++) {
					m2[i][j] = a[i][j] + m[i][j];
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
			m[0][0] = a[1][1] * a[2][2] - a[2][1] * a[1][2];
			m[0][1] = a[0][2] * a[2][1] - a[0][1] * a[2][2];
			m[0][2] = a[0][1] * a[1][2] - a[0][2] * a[1][1];
			m[1][0] = a[1][2] * a[2][0] - a[1][0] * a[2][2];
			m[1][1] = a[0][0] * a[2][2] - a[0][2] * a[2][0];
			m[1][2] = a[1][0] * a[0][2] - a[0][0] * a[1][2];
			m[2][0] = a[1][0] * a[2][1] - a[2][0] * a[1][1];
			m[2][1] = a[2][0] * a[0][1] - a[0][0] * a[2][1];
			m[2][2] = a[0][0] * a[1][1] - a[1][0] * a[0][1];
			return m / det;
		}

		Matrix3 Matrix3::identity() {
			Matrix3 m;
			for (register int i = 0; i < 3; i++) {
				m[i][i] = 1.0f;
			}
			return m;
		}

		Matrix3 Matrix3::from_rotation(Vector<3> r) {
			//Rodrigues transform
			//https://blog.csdn.net/wu_wenhuan/article/details/52588921

			float ¦È = r.norm();

			if (¦È <= 0) {
				return Matrix3::identity();
			}

			r = r / ¦È;
			float cos = cosf(¦È), sin = sinf(¦È);

			Matrix3 m;
			m[0][1] = -r[2]; // a12 = -rz
			m[0][2] = r[1];  // a13 = ry
			m[1][0] = r[2];  // a21 = rz
			m[1][2] = -r[0]; // a23 = -rx
			m[2][0] = -r[1]; // a31 = -ry
			m[2][1] = r[0];  // a32 = rx;

			Matrix3 rrT;
			for (register int i = 0; i < 3; i++) {
				for (register int j = 0; j < 3; j++) {
					rrT[i][j] = r[i] * r[j];
				}
			}
			Matrix3 R = Matrix3::identity() * cos + rrT *(1 - cos) + m * sin;

			return R;
		}

		Matrix3 Matrix3::from_scale(float scale) {
			Matrix3 m;
			for (register int i = 0; i < 3; i++) {
				m[i][i] = scale;
			}
			return m;
		}

	}
}
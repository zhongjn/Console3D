//#include "my3d.h"
//#include <math.h>
//
//namespace console3d {
//	namespace core {
//		Vector<3>::Vector<3>()
//		{
//		}
//		Vector<3>::Vector<3>(float x, float y, float z)
//		{
//			a[0] = x;
//			a[1] = y;
//			a[2] = z;
//		}
//		float Vector<3>::norm() {
//			float accum = 0.0f;
//			for (int i = 0; i < 3; i++) {
//				accum += powf(a[i], 2);
//			}
//			return sqrtf(accum);
//		}
//		Vector<3> Vector<3>::operator*(float factor) {
//			Vector<3> v2;
//			for (int i = 0; i < 3; i++) {
//				v2.a[i] = a[i] * factor;
//			}
//			return v2;
//		}
//		Vector<3> Vector<3>::operator/(float factor)
//		{
//			return (*this) * (1.0f / factor);
//		}
//		Vector<3> Vector<3>::operator+(Vector<3> vec) {
//			Vector<3> v2;
//			for (int i = 0; i < 3; i++) {
//				v2.a[i] = a[i] + vec.a[i];
//			}
//			return v2;
//		}
//
//		Vector<3> Vector<3>::operator-() {
//			Vector<3> v2;
//			for (int i = 0; i < 3; i++) {
//				v2.a[i] = -a[i];
//			}
//			return v2;
//		}
//
//		Vector<3> Vector<3>::operator-(Vector<3> vec) {
//			return (*this) + (-vec);
//		}
//
//
//		float Vector<3>::operator*(Vector<3> vec) {
//			return dot(vec);
//		}
//
//		float Vector<3>::dot(Vector<3> vec) {
//			float accum = 0.0f;
//			for (int i = 0; i < 3; i++) {
//				accum += a[i] * vec.a[i];
//			}
//			return accum;
//		}
//
//		Vector<3> Vector<3>::cross(Vector<3> vec) {
//			// TODO:
//			// cross product
//			return Vector<3>();
//		}
//
//	}
//}
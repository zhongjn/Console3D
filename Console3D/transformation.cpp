#include "core.h"
namespace console3d {
	namespace core {

		Transformation::Transformation() {
			linear = Matrix3::identity();
		}

		Transformation::Transformation(Matrix3 linear, Vector<3> translation)
			: linear(linear), translation(translation) {
		}

		Transformation Transformation::inverse() {
			Matrix3 inv = linear.inverse();
			Transformation out;
			out.linear = inv;
			out.translation = -(inv * translation);
			return out;
		}

		Transformation Transformation::operator*(Transformation t) {
			Transformation out;
			out.linear = linear * t.linear;
			out.translation = linear * t.translation + translation;
			return out;
		}

		Vector<3> Transformation::apply(Vector<3> v, bool affine) {
			Vector<3> out;
			out = linear * v;
			if (affine) {
				out = translation + out;
			}
			return out;
		}

		Transformation Transformation::scale(float factor) {
			Transformation out;
			out.linear = Matrix3::from_scale(factor);
			return out * (*this);
		}

		Transformation Transformation::rotate(int axis, float theta) {
			if (axis >= 0 && axis < 3) {
				Vector<3> r;
				r[axis] = theta;
				Transformation out(Matrix3::from_rotation(r), Vector<3>());
				return  out * (*this);
			}
			else {
				return  *this;
			}
		}

		Transformation Transformation::translate(Vector<3> vec)
		{
			Transformation out;
			out.translation = vec;
			return  out * (*this);
		}
	}
}

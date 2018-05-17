#pragma once
#include <stdarg.h>
#include <math.h>

using namespace std;

namespace console3d {
	namespace core {

		const int X = 0, Y = 1, Z = 2;
		const int HORIZONTAL = 0, VERTICAL = 1, DEPTH = 2;
		const int U = 0, V = 0;

		template<int _size>
		class Vector {
		private:
			float a[_size] = { 0.0f };
		public:
			Vector() {};
			Vector(float a0, ...) {
				va_list ap;
				va_start(ap, a0);
				a[0] = a0;
				for (int i = 1; i < _size; i++) {
					a[i] = (float)va_arg(ap, double);
				}
				va_end(ap);
			}

			float& operator[](int i) {
				return a[i];
			}

			float norm() {
				float accum = 0.0f;
				for (int i = 0; i < _size; i++) {
					accum += powf(a[i], 2);
				}
				return sqrtf(accum);
			}
			Vector<_size> operator*(float factor) {
				Vector<_size> v2;
				for (int i = 0; i < _size; i++) {
					v2[i] = a[i] * factor;
				}
				return v2;
			}
			Vector<_size> operator/(float factor) {
				return (*this) * (1.0f / factor);
			}
			Vector<_size> operator+(Vector<_size> vec) {
				Vector<_size> v2;
				for (int i = 0; i < _size; i++) {
					v2[i] = a[i] + vec[i];
				}
				return v2;
			}
			Vector<_size> operator-() {
				Vector<_size> v2;
				for (int i = 0; i < _size; i++) {
					v2[i] = -a[i];
				}
				return v2;
			}
			Vector<_size> operator-(Vector<_size> vec) {
				return (*this) + (-vec);
			}
			float dot(Vector<_size> vec) {
				float accum = 0.0f;
				for (int i = 0; i < _size; i++) {
					accum += a[i] * vec[i];
				}
				return accum;
			}
			float operator*(Vector<_size> vec) {
				return dot(vec);
			}
		};

		typedef Vector<2> UV;
		typedef Vector<3> ScreenCoord;


		//变换矩阵
		class Matrix3 {
		private:
			float a[3][3] = { 0.0f };
		public:
			float* operator[](int i);
			Vector<3> operator*(Vector<3> vec);
			Matrix3 operator*(Matrix3 m);
			Matrix3 operator*(float factor);
			Matrix3 operator/(float factor);
			Matrix3 operator+(Matrix3 m);
			Matrix3 inverse();
			static Matrix3 identity();
			static Matrix3 from_rotation(Vector<3> rotation);
			static Matrix3 from_scale(float scale);
		};


		class Transformation {
		public:
			Transformation();
			Transformation(Matrix3 linear, Vector<3> translation);
			Transformation inverse();
			Transformation operator*(Transformation t);
			Vector<3> apply(Vector<3> v, bool affine);
			Transformation scale(float scale);
			Transformation rotate(int axis, float theta);
			Transformation translate(Vector<3> vec);
			Matrix3 linear;
			Vector<3> translation;
		};

		


		//颜色
		struct Color {
			unsigned char r, g, b;
		};


		//顶点
		struct Vertex {
			Vector<3> position;
			Color color;
		};

		struct Size {
			float width, height;
		};


		//3D物体最基本的单位
		class Object {
		public:
			Transformation transformation;
		};


		//后续再支持，先实现Line
		//class Polyhedron : Object { 
		//	Vertex* vertexes;
		//	int vertexes_count;
		//};


		//线框
		class Line : public Object {
		public:
			Vector<3> orientation;
			Color color1 = { 0xFF, 0xFF, 0xFF };
			Color color2 = { 0xFF, 0xFF, 0xFF };
		};


		//相机
		class Camera : public Object {
		public:
			Camera();
			Camera(Transformation trans, Size lens);
			Size lens_size;
			float depth_minimum = 0.0f;
		};


		//像素
		class Pixel {
		public:
			float depth = 0.0; //深度
			Color color = { 0, 0, 0 };
		};


		class Core3DContext {
		public:
			short height, width;
			Core3DContext(short height, short width); //根据画面高度宽度初始化pixels
			Pixel* pixels;
			void draw_begin(); //开始画一帧，还原pixels
			void draw_end(); //结束画一帧
			void draw_line(Line &line); //将一条线光栅化到像素上
			Transformation world;
			Camera camera;
		private:
			Transformation camera_detransform;
			ScreenCoord project_to_screen(Vector<3> position);
			Matrix3 screen_coord_to_plane;
		};

	}
}
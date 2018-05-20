#pragma once
#include <stdarg.h>
#include <math.h>
#include <vector>
#include <type_traits>

//using namespace std;

namespace console3d {
	namespace core {

		const int X = 0, Y = 1, Z = 2;
		//const int DEPTH = 2;
		//const int X = 0, Y = 1, DEPTH = 2;
		//const int U = 0, V = 0;

		template<int _size>
		class Vector {
		private:
			float a[_size];
		public:
			Vector() {
				for (int i = 0; i < _size; i++) {
					a[i] = 0.0f;
				}
			};

			template <typename... Args>
			Vector(Args... args) : a{ float(args)... } {
				static_assert(sizeof...(Args) == _size, "Wrong number of arguments!");
			}


			inline float& operator[](int i) {
				return a[i];
			}

			inline float norm() {

				float accum = 0.0f;
				for (int i = 0; i < _size; i++) {
					accum += a[i] * a[i];
				}
				return sqrtf(accum);
			}
			inline Vector<_size> operator*(float factor) {
				Vector<_size> v2;
				for (int i = 0; i < _size; i++) {
					v2[i] = a[i] * factor;
				}
				return v2;
			}
			inline Vector<_size> operator/(float factor) {
				return (*this) * (1.0f / factor);
			}
			inline Vector<_size> operator+(Vector<_size> vec) {
				Vector<_size> v2;
				for (int i = 0; i < _size; i++) {
					v2[i] = a[i] + vec[i];
				}
				return v2;
			}
			inline Vector<_size> operator-() {
				Vector<_size> v2;
				for (int i = 0; i < _size; i++) {
					v2[i] = -a[i];
				}
				return v2;
			}
			inline Vector<_size> operator-(Vector<_size> vec) {
				return (*this) + (-vec);
			}
			inline float dot(Vector<_size> vec) {
				float accum = 0.0f;
				for (int i = 0; i < _size; i++) {
					accum += a[i] * vec[i];
				}
				return accum;
			}
			inline float operator*(Vector<_size> vec) {
				return dot(vec);
			}
		};

		typedef Vector<2> UV;
		typedef Vector<2> ScreenCoordXY;
		typedef Vector<3> ScreenCoordXYZ;
		typedef Vector<3> Color;
		typedef Vector<3> Position;

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


		//顶点
		struct Vertex {
		public:
			Position position;
			Color color;
		};

		struct VertexCache {
		public:
			Vertex vertex;
			ScreenCoordXYZ coord;
			VertexCache(Vertex vertex);
		};


		struct Size {
			float width, height;
		};

		struct VertexTriangle {
		public:
			int index[3] = { 0 };
			VertexTriangle();
			VertexTriangle(int i0, int i1, int i2);
		};


		//3D物体最基本的单位
		class Object {
		public:
			Transformation transformation;
		};


		class Mesh : public Object {
		public:
			std::vector<Vertex> vertexes;
			std::vector<VertexTriangle> triangles;
			Mesh(std::vector<Vertex> vertexes, std::vector<VertexTriangle> triangles);
		};


		////线框
		//class Line : public Object {
		//public:
		//	Vector<3> orientation;
		//	Color color1 = { 0xFF, 0xFF, 0xFF };
		//	Color color2 = { 0xFF, 0xFF, 0xFF };
		//};


		//相机
		class Camera : public Object {
		public:
			Camera();
			Camera(Transformation trans, Size lens);
			Size lens_size;
			float depth_minimum = 1.0f;
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
			Core3DContext(short width, short height); //根据画面高度宽度初始化pixels
			Pixel* pixels;
			void scene_begin(); //开始画一帧，还原pixels
			void scene_end(); //结束画一帧
			//void draw_line(Line &line); //将一条线光栅化到像素上
			void draw_mesh(Mesh &mesh);
			Transformation transformation_world;
			Camera camera;
		private:
			std::vector<VertexCache> buffer_vertexes;
			std::vector<VertexTriangle> buffer_triangles;
			Transformation transformation_combined;
			void rasterize();
			ScreenCoordXYZ project_to_screen(Vector<3> position);
		};

	}
}
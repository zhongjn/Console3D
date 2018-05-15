#pragma once

using namespace std;

namespace console3d {
	namespace core {

		//二维向量
		class Vector2 {
		public:
			float a[2];
		};


		//三维向量
		class Vector3 {
		public:
			float a[3];
			float norm();
			Vector3 operator*(float factor);
			Vector3 operator+(Vector3 vec);
			Vector3 operator-(Vector3 vec);
			Vector3 operator-();
			float operator*(Vector3 vec);
			float dot(Vector3 vec);
			Vector3 cross(Vector3 vec);
		};


		//变换矩阵
		class Matrix3 {
		public:
			float a[3][3] = { 0.0 };
			Vector3 operator*(Vector3 vec);
			Matrix3 operator*(Matrix3 m);
			static Matrix3 identity();
			static Matrix3 from_rotation(Vector3 rotation);
			static Matrix3 from_scale(float scale);
		};


		//颜色
		struct Color {
		public:
			unsigned char r, g, b;
		};


		//顶点
		struct Vertex {
		public:
			Vector3 position;
			Color color;
		};


		//3D物体最基本的单位
		class Object {
		public:
			Vector3 position;
			Vector3 rotation;
		};


		//后续再支持，先实现Line
		//class Polyhedron : Object { 
		//	Vertex* vertexes;
		//	int vertexes_count;
		//};


		//线框
		class Line : Object {
		public:
			Vector3 orientation;
		};


		//相机
		class Camera : Object {
		public:
			Vector2 angle;
			float depth_minimum;
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
			void draw_begin(Camera& camera); //开始画一帧，还原pixels
			void draw_end(); //结束画一帧
			void draw_line(Line &line); //将一条线光栅化到像素上
		private:
			Matrix3 camera_derotation;
		};

	}
}
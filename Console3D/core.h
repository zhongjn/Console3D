#pragma once

#include <vector>
#include <memory>
#include <string>

using namespace std;

namespace console3d {
	namespace core {

		//二维向量
		class Vector2 {
			float x, y;
		};


		//三维向量
		class Vector3 {
			float x, y, z;
		};


		//仿射变换矩阵
		class AffineMatrix {
			float a[4][4];
		};


		//颜色
		class Color {
			unsigned char r, g, b;
		};


		//顶点
		class Vertex {
			Vector3 position;
			Color color;
		};


		//3D物体最基本的单位
		class Object {
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
			Vector3 orientation;
		};


		//相机
		class Camera : Object {
			Vector2 angle;
			float depth_minimum;
		};


		//像素
		class Pixel {
			float depth; //深度
			Color color;
		};


		class Core3DContext {
			Core3DContext(short height, short width); //根据画面高度宽度初始化pixels
			Pixel* pixels;
			void draw_begin(Camera& camera); //开始画一帧，还原pixels
			void draw_end(); //结束画一帧
			void draw_line(Line& line); //将一条线光栅化到像素上
		};

	}
}
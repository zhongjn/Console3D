#pragma once

#include <vector>
#include <memory>
#include <string>

using namespace std;

namespace console3d {
	namespace core {

		//��ά����
		class Vector2 {
			float x, y;
		};


		//��ά����
		class Vector3 {
			float x, y, z;
		};


		//����任����
		class AffineMatrix {
			float a[4][4];
		};


		//��ɫ
		class Color {
			unsigned char r, g, b;
		};


		//����
		class Vertex {
			Vector3 position;
			Color color;
		};


		//3D����������ĵ�λ
		class Object {
			Vector3 position;
			Vector3 rotation;
		};


		//������֧�֣���ʵ��Line
		//class Polyhedron : Object { 
		//	Vertex* vertexes;
		//	int vertexes_count;
		//};


		//�߿�
		class Line : Object {
			Vector3 orientation;
		};


		//���
		class Camera : Object {
			Vector2 angle;
			float depth_minimum;
		};


		//����
		class Pixel {
			float depth; //���
			Color color;
		};


		class Core3DContext {
			Core3DContext(short height, short width); //���ݻ���߶ȿ�ȳ�ʼ��pixels
			Pixel* pixels;
			void draw_begin(Camera& camera); //��ʼ��һ֡����ԭpixels
			void draw_end(); //������һ֡
			void draw_line(Line& line); //��һ���߹�դ����������
		};

	}
}
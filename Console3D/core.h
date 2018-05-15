#pragma once

using namespace std;

namespace console3d {
	namespace core {

		//��ά����
		class Vector2 {
		public:
			float a[2];
		};


		//��ά����
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


		//�任����
		class Matrix3 {
		public:
			float a[3][3] = { 0.0 };
			Vector3 operator*(Vector3 vec);
			Matrix3 operator*(Matrix3 m);
			static Matrix3 identity();
			static Matrix3 from_rotation(Vector3 rotation);
			static Matrix3 from_scale(float scale);
		};


		//��ɫ
		struct Color {
		public:
			unsigned char r, g, b;
		};


		//����
		struct Vertex {
		public:
			Vector3 position;
			Color color;
		};


		//3D����������ĵ�λ
		class Object {
		public:
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
		public:
			Vector3 orientation;
		};


		//���
		class Camera : Object {
		public:
			Vector2 angle;
			float depth_minimum;
		};


		//����
		class Pixel {
		public:
			float depth = 0.0; //���
			Color color = { 0, 0, 0 };
		};


		class Core3DContext {
		public:
			short height, width;
			Core3DContext(short height, short width); //���ݻ���߶ȿ�ȳ�ʼ��pixels
			Pixel* pixels;
			void draw_begin(Camera& camera); //��ʼ��һ֡����ԭpixels
			void draw_end(); //������һ֡
			void draw_line(Line &line); //��һ���߹�դ����������
		private:
			Matrix3 camera_derotation;
		};

	}
}
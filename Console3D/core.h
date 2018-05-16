#pragma once

using namespace std;

namespace console3d {
	namespace core {

		const int X = 0, Y = 1, Z = 2;


		//��ά����
		class Vector3 {
		public:
			Vector3();
			Vector3(float x, float y, float z);
			float a[3] = { 0.0f };
			float norm();
			Vector3 operator*(float factor);
			Vector3 operator/(float factor);
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
			float a[3][3] = { 0.0f };
			Vector3 operator*(Vector3 vec);
			Matrix3 operator*(Matrix3 m);
			Matrix3 operator*(float factor);
			Matrix3 operator/(float factor);
			Matrix3 operator+(Matrix3 m);
			Matrix3 inverse();
			static Matrix3 identity();
			static Matrix3 from_rotation(Vector3 rotation);
			static Matrix3 from_scale(float scale);
		};


		class Transformation {
		public:
			Transformation();
			Transformation(Matrix3 linear, Vector3 translation);
			Transformation inverse();
			Transformation operator*(Transformation t);
			Vector3 apply(Vector3 v, bool affine);
			Transformation scale(float scale);
			Transformation rotate(int axis, float theta);
			Transformation translate(Vector3 vec);
			Matrix3 linear;
			Vector3 translation;
		};



		//��ɫ
		struct Color {
			unsigned char r, g, b;
		};


		//����
		struct Vertex {
			Vector3 position;
			Color color;
		};

		struct Size {
			float width, height;
		};


		//3D����������ĵ�λ
		class Object {
		public:
			Transformation transformation;
		};


		//������֧�֣���ʵ��Line
		//class Polyhedron : Object { 
		//	Vertex* vertexes;
		//	int vertexes_count;
		//};


		//�߿�
		class Line : public Object {
		public:
			Vector3 orientation;
			Color color1 = { 0xFF, 0xFF, 0xFF };
			Color color2 = { 0xFF, 0xFF, 0xFF };
		};


		//���
		class Camera : public Object {
		public:
			Camera();
			Camera(Transformation trans, Size lens);
			Size lens_size;
			float depth_minimum = 0.0f;
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
			void draw_begin(); //��ʼ��һ֡����ԭpixels
			void draw_end(); //������һ֡
			void draw_line(Line &line); //��һ���߹�դ����������
			Transformation world;
			Camera camera;
		private:
			Transformation camera_detransform;
			Vector3 project_to_screen_coord(Vector3 position);
			Matrix3 screen_coord_to_plane;
		};

	}
}
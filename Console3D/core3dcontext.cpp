#define _USE_MATH_DEFINES
#include "core.h"
#include <cfloat>
#include <algorithm>
#include <functional>
#include <cmath>

namespace console3d {
	namespace core {

		const int reserve_vertex_count = 512;
		const int reserve_triangle_count = 256;
		const float basis_abs_sin_threshold = 1E-3;
		const float horizontal_abs_dy_threshold = 1E-2;

#pragma region utils

		struct bilinear_info {
			ScreenCoordXY base;
			Vector<2> basis[2];
			float z[3];
			bool ok;
			float det;
		};


		bilinear_info prepare_bilinear_info(ScreenCoordXYZ s[3]);
		ScreenCoordXY xyz_to_xy(ScreenCoordXYZ s);
		BilinearCoeffient get_corrected_bilinear(ScreenCoordXY u, bilinear_info &info);
		Vector<2> vec2_decompose_fast(Vector<2> b[2], Vector<2> vec, float det);
		Vector<2> vec2_decompose(Vector<2> b[2], Vector<2> vec, bool &ok);
		Vector<2> vec2_decompose(Vector<2> basis[2], Vector<2> vec, bool &ok);
		Vector<3> vec3_crossproduct(Vector<3> v[2]);
		Color color_mix(Color c1, Color c2);

		template<typename T>
		inline void boundary_limit(T& x, T min, T max) {
			if (x < min) {
				x = min;
			}
			else if (x > max) {
				x = max;
			}
		}

		template<typename TS, typename F, typename TV = std::result_of<F(TS)>::type>
		inline TV apply_bilinear(TS sources[3], Vector<3> coefficients, F& func) {
			TV accum = TV();
			for (int i = 0; i < 3; i++) {
				accum = accum + func(sources[i]) * coefficients[i];
			}
			return accum;
		}

		template<typename TS, typename F, typename TV = std::result_of<F(TS)>::type>
		TV apply_bilinear(TS sources[], int index[3], Vector<3> coefficients, F& func) {
			TV accum = TV();
			for (int i = 0; i < 3; i++) {
				accum = accum + func(sources[index[i]]) * coefficients[i];
			}
			return accum;
		}

#pragma endregion

		Core3DContext::Core3DContext(short w, short h) {
			height = h;
			width = w;
			buffer_pixels = new Pixel[height * width];
			output = new Color[height * width];
			triangles.reserve(reserve_triangle_count);
			vertexes.reserve(reserve_vertex_count);
		}

		void Core3DContext::scene_begin() {
			for (int i = 0; i < height * width; i++) {
				buffer_pixels[i].triangle = nullptr;
				buffer_pixels[i].depth = FLT_MAX;
			}
			transformation_combined = camera.transformation.inverse() * transformation_world;
			triangles.clear();
			vertexes.clear();
		}

		void Core3DContext::scene_end() {

			for (auto &vc : vertexes) {
				vc.coord = project_to_screen(vc.position);
			}
			vertex_shade();
			cull();
			clip_z();
			rasterize();
			pixel_shade();
		}



		void Core3DContext::draw_mesh(Mesh &mesh) {

			int vi = vertexes.size();
			Transformation trans = transformation_combined * mesh.transformation;

			for (auto v : mesh.vertexes) {
				//副本v
				v.position = trans.apply(v.position, true);
				vertexes.emplace_back(v); // 就地构造VertexCache
			}

			for (auto t : mesh.triangles) {
				//副本t
				for (int i = 0; i < 3; i++) {
					t.index[i] += vi;
				}
				t.roughness = mesh.roughness;
				triangles.emplace_back(t);
			}
		}

		void Core3DContext::set_ambient_light(AmbientLight &light)
		{
			ambient_light = light;
		}

		void Core3DContext::set_point_light(std::vector<PointLight> &lights)
		{
			point_lights = lights; // 深拷贝一份
			for (auto &light : point_lights) {
				light.position = transformation_combined.apply(light.position, true);
			}
		}

		void Core3DContext::vertex_shade()
		{
		}

		void Core3DContext::cull()
		{
		}

		void Core3DContext::clip_z()
		{
		}

		void Core3DContext::rasterize() {
			for (auto &triangle : triangles) {
				ScreenCoordXYZ coords[3];


				Vertex mvertexes[3];
				for (int i = 0; i < 3; i++) {
					auto &vc = vertexes[triangle.index[i]];
					coords[i] = vc.coord;
					mvertexes[i] = vc;
				}

				auto info = prepare_bilinear_info(coords);
				if (!info.ok) continue;

				Vector<3> vecs[3] = { coords[1] - coords[0] , coords[2] - coords[1], coords[0] - coords[2] };
				int horizontal_index = -1;
				bool all_horizontal = false;
				for (int j = 0; j < 3; j++) {
					if (fabs(vecs[j][Y]) <= horizontal_abs_dy_threshold) {
						if (horizontal_index == -1) {
							horizontal_index = j;
						}
						else {
							all_horizontal = true;
							break;
						}
					}
				}

				if (all_horizontal) {
					continue;
				}

				//确定三点中最大与最小的x, y值
				float y_min = FLT_MAX, y_max = FLT_MIN;
				for (int j = 0; j < 3; j++) {
					float y = coords[j][Y];
					if (y < y_min) {
						y_min = y;
					}
					if (y > y_max) {
						y_max = y;
					}
				}

				int y_start = int(ceilf(y_min) + 0.5); // +0.5四舍五入，防止浮点误差
				int y_end = int(floorf(y_max) + 0.5);
				boundary_limit(y_start, 0, height - 1);
				boundary_limit(y_end, 0, height - 1);

				for (int px_y = y_start; px_y <= y_end; px_y++) {
					int count = 0;
					float x_inter[3];
					for (int j = 0; j < 3; j++) {
						if (j == horizontal_index) continue; // 扫描线不与水平线求交
						ScreenCoordXYZ &from = coords[j];
						ScreenCoordXYZ &to = coords[(j + 1) % 3];
						if ((px_y - from[Y]) * (px_y - to[Y]) <= 0) { //交点存在
							float k = (px_y - coords[j][Y]) / vecs[j][Y];
							x_inter[count] = vecs[j][X] * k + coords[j][X];
							count++;
						}
					}

					float x_inter_min = FLT_MAX, x_inter_max = FLT_MIN;
					for (int j = 0; j < count; j++) {
						float x = x_inter[j];
						if (x < x_inter_min) {
							x_inter_min = x;
						}
						if (x > x_inter_max) {
							x_inter_max = x;
						}
					}

					if (count >= 2) { // 扫到了！
									  //if (x_inter[0] > x_inter[1]) std::swap(x_inter[0], x_inter[1]);
						int x_start, x_end;
						x_start = int(ceilf(x_inter_min) + 0.5);
						x_end = int(floorf(x_inter_max) + 0.5);

						boundary_limit(x_start, 0, width - 1);
						boundary_limit(x_end, 0, width - 1);

						for (int px_x = x_start; px_x <= x_end; px_x++) {
							ScreenCoordXY u(px_x, px_y);
							Vector<3> k = get_corrected_bilinear(u, info);
							int bb = 0;

							float depth = apply_bilinear(coords, k, [](ScreenCoordXYZ coord) {return coord[Z]; });

							Pixel &p = get_pixel(px_x, px_y);
							if (depth >= camera.depth_minimum && depth < p.depth) {
								p.triangle = &triangle;
								p.bilinear_coefficient = k;
								p.depth = depth;
							}
						}
					}
				}
			}

		}

		void Core3DContext::pixel_shade()
		{
			int pl_n = point_lights.size();
			PointLight* pl = &point_lights[0];

			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					Pixel &p = get_pixel(i, j);
					Color out;
					if (p.triangle != nullptr) {

						Color origin = apply_bilinear(vertexes.data(), p.triangle->index, p.bilinear_coefficient, [](Vertex v) { return v.color; });
						compute_triangle_normal(p.triangle);
						Vector<3> normal = p.triangle->normal;

						Position pos = apply_bilinear(vertexes.data(), p.triangle->index, p.bilinear_coefficient, [](Vertex v) { return v.position; });

						out = origin * 0.3;

						
						for (int ip = 0; ip < pl_n; ip++) {
							PointLight &light = pl[ip];
							Vector<3> light_in = (light.position - pos).normalize(); // 入射向量，指向光源
							Vector<3> obsv = -pos.normalize(); // 观察向量，指向观察者
							
							float t1 = light_in.dot(normal);
							float t2 = obsv.dot(normal);
							float test = t1 * t2;  // 若>0，则光源与观察者在平面同一侧
							if (test > 0) {
								if (t2 < 0) {
									p.triangle->normal = normal = -normal;
								}
								//Vector<3> light_out = normal * (light_in.dot(normal) * 2) - light_in;

								// 反射光，土制的正态模型，试试效果
								//Vector<3> half = (light_in + obsv).normalize();
								//float cos = light_out.dot(obsv);
								//float angle = acosf(cos);
								//float k_refl = expf(-angle * angle / 2 / roughness / roughness) / 2 / M_PI / roughness; // 反射系数
								//
								//// 几何修正
								//float cg = 2 * half.dot(normal) / half.dot(obsv);
								//float cc = fminf(normal.dot(obsv), normal.dot(light_in));
								//float g = fminf(1, cc * cg);
								//k_refl *= g;
								//out = out + light.color * (k_refl * light.intensity);

								// Lambert
								float cos = light_in.dot(normal);
								auto vt = light.position - pos;
								float decay = 1 / vt.dot(vt);
								out = out + light.color * (cos * light.intensity * decay);
							}
						}

						// 环境光
						out = out + ambient_light.color * ambient_light.intensity;
					}

					output[j * width + i] = out;
				}
			}
		}

		void Core3DContext::compute_triangle_normal(Triangle * triangle)
		{
			if (triangle != nullptr) {
				if (!triangle->normal_computed) {
					Vector<3> v[2];
					for (int i = 0; i < 2; i++) {
						v[i] = vertexes[triangle->index[i + 1]].position - vertexes[triangle->index[0]].position;
					}
					Vector<3> out;
					out = vec3_crossproduct(v);
					triangle->normal = out / out.norm();
					triangle->normal_computed = true;
				}
			}
		}

		Pixel & Core3DContext::get_pixel(int i, int j)
		{
			return buffer_pixels[j * width + i];
		}

		ScreenCoordXYZ Core3DContext::project_to_screen(Position pos) {
			// (x,y)=ScreenCoord, x=vertical, y=horizontal, TopLeft=(0,0), BottomRight=(width,height)
			// z = depth
			Vector<3> temp;
			float sc = pos[2];
			if (sc < 0) {
				sc = -sc;
			}
			temp[0] = pos[0] / sc / camera.lens_size.width + 0.5f;
			temp[1] = pos[1] / sc / camera.lens_size.height + 0.5f;

			ScreenCoordXYZ coord;
			coord[Z] = pos[2];
			coord[X] = (float)width * (1 - temp[0]);
			coord[Y] = (float)height * (1 - temp[1]);
			return coord;
		}



		ScreenCoordXY xyz_to_xy(ScreenCoordXYZ s) {
			Vector<2> ret;
			ret[0] = s[0];
			ret[1] = s[1];
			return ret;
		}



		bilinear_info prepare_bilinear_info(ScreenCoordXYZ s[3]) {

			bilinear_info info;

			info.ok = true;

			Vector<2> vec2[3];
			for (int i = 0; i < 3; i++) {
				vec2[i] = xyz_to_xy(s[i]);
				info.z[i] = s[i][Z];
			}

			info.base = vec2[0];

			info.basis[0] = vec2[1] - vec2[0];
			info.basis[1] = vec2[2] - vec2[0];

			auto b = info.basis;

			info.det = b[0][0] * b[1][1] - b[1][0] * b[0][1];
			float sin = info.det / b[0].norm() / b[1].norm();
			if (fabs(sin) <= basis_abs_sin_threshold) {
				info.ok = false;
			}

			return info;

		}


		BilinearCoeffient get_corrected_bilinear(ScreenCoordXY u, bilinear_info &info) {
			if (info.ok) {
				Vector<2> vec = u - info.base;
				Vector<2> k12 = vec2_decompose_fast(info.basis, vec, info.det);
				Vector<3> k(1 - k12[0] - k12[1], k12[0], k12[1]);
				Vector<3> out;
				float denom = 0.0f;
				for (int i = 0; i < 3; i++) {
					float f = k[i] / info.z[i];
					denom += f;
					out[i] = f;
				}
				return out / denom;
			}
			else {
				return Vector<3>();
			}
		}

		Vector<2> vec2_decompose_fast(Vector<2> b[2], Vector<2> vec, float det) {
			Vector<2> ret;
			ret[0] = (b[1][1] * vec[0] - b[1][0] * vec[1]);
			ret[1] = (-b[0][1] * vec[0] + b[0][0] * vec[1]);
			return ret / det;
		}

		Vector<2> vec2_decompose(Vector<2> b[2], Vector<2> vec, bool &ok)
		{
			float det = b[0][0] * b[1][1] - b[1][0] * b[0][1];
			float sin = det / b[0].norm() / b[1].norm();
			if (fabs(sin) <= basis_abs_sin_threshold) {
				ok = false;
				return Vector<2>();
			}
			return vec2_decompose_fast(b, vec, det);

		}

		Vector<3> vec3_crossproduct(Vector<3> v[2])
		{
			Vector<3> out;
			out[0] = v[0][1] * v[1][2] - v[0][2] * v[1][1];
			out[1] = v[0][2] * v[1][0] - v[0][0] * v[1][2];
			out[2] = v[0][0] * v[1][1] - v[0][1] * v[1][0];
			return out;
		}
		Color color_mix(Color c1, Color c2)
		{
			Color out;
			for (int i = 0; i < 3; i++) {
				out[i] = c1[i] * c2[i]; // 交给编译器优化
			}
			return out / 255.0f;
		}
	}
}
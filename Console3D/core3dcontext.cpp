#include "core.h"
#include <cfloat>
#include <algorithm>
#include <functional>
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
		Vector<3> get_corrected_bilinear(ScreenCoordXY u, bilinear_info &info);
		Vector<2> vec2_decompose_fast(Vector<2> b[2], Vector<2> vec, float det);
		Vector<2> vec2_decompose(Vector<2> b[2], Vector<2> vec, bool &ok);

#pragma endregion

		template<typename T>
		void boundary_limit(T& x, T min, T max) {
			if (x < min) {
				x = min;
			}
			else if (x > max) {
				x = max;
			}
		}

		template<typename TS, typename F, typename TV = std::result_of<F(TS)>::type>
		TV apply_bilinear(TS sources[3], Vector<3> coeffients, F& func) {
			TV accum = TV();
			for (int i = 0; i < 3; i++) {
				accum = accum + func(sources[i]) * coeffients[i];
			}
			return accum;
		}

		Vector<2> vec2_decompose(Vector<2> basis[2], Vector<2> vec, bool &ok);
		Vector<2> xyz_to_xy(ScreenCoordXYZ s);

		Core3DContext::Core3DContext(short w, short h) {
			height = h;
			width = w;
			pixels = new Pixel[height * width];
			buffer_triangles.reserve(reserve_triangle_count);
			buffer_vertexes.reserve(reserve_vertex_count);
		}

		void Core3DContext::scene_begin() {
			for (int i = 0; i < height * width; i++) {
				pixels[i].color = { 0, 0, 0 };
				pixels[i].depth = FLT_MAX;
			}
			transformation_combined = camera.transformation.inverse() * transformation_world;
			buffer_triangles.clear();
			buffer_vertexes.clear();
		}

		void Core3DContext::scene_end() {

			for (auto &vc : buffer_vertexes) {
				vc.coord = project_to_screen(vc.vertex.position);
			}
			// 光栅化三角形
			rasterize();
		}


		void Core3DContext::draw_mesh(Mesh &mesh) {

			int vi = buffer_vertexes.size();
			Transformation trans = transformation_combined * mesh.transformation;

			for (auto v : mesh.vertexes) {
				//副本v
				v.position = trans.apply(v.position, true);
				buffer_vertexes.emplace_back(v); // 就地构造VertexCache
			}

			for (auto &t : mesh.triangles) {
				buffer_triangles.emplace_back(t.index[0] + vi, t.index[1] + vi, t.index[2] + vi);
			}


			//std::vector<Vertex> vertexes_all(mesh.vertexes);
			//if (vertexes_all.size() >= 3) { // 至少形成一个三角形
			//	auto trans = mesh.transformation;
			//	std::vector<ScreenCoordXYZ> coords_all;
			//	coords_all.reserve(vertexes_all.size());
			//	for (auto &v : vertexes_all) {
			//		v.position = trans.apply(v.position, true);
			//		coords_all.emplace_back(project_to_screen(v.position));
			//	}
			//	for (auto &triangle : mesh.triangles) {
			//		ScreenCoordXYZ coords[3];
			//		Vertex vertexes[3];
			//		for (int i = 0; i < 3; i++) {
			//			coords[i] = coords_all[triangle.index[i]];
			//			vertexes[i] = vertexes_all[triangle.index[i]];
			//		}

			//		Vector<3> vecs[3] = { coords[1] - coords[0] , coords[2] - coords[1], coords[0] - coords[2] };
			//		int horizontal_index = -1;
			//		bool all_horizontal = false;
			//		for (int j = 0; j < 3; j++) {
			//			if (vecs[j][Y] == 0.0f) {
			//				if (horizontal_index == -1) {
			//					horizontal_index = j;
			//				}
			//				else {
			//					all_horizontal = true;
			//					break;
			//				}
			//			}
			//		}
			//		if (all_horizontal) continue;


			//		//确定三点中最大与最小的y值
			//		float ymin = FLT_MAX, ymax = FLT_MIN;
			//		for (int j = 0; j < 3; j++) {
			//			float y = coords[j][Y];
			//			if (y < ymin) {
			//				ymin = y;
			//			}
			//			if (y > ymax) {
			//				ymax = y;
			//			}
			//		}


			//		//
			//		int y_start = int(ceilf(ymin) + 0.5); // +0.5四舍五入，防止浮点误差
			//		int y_end = int(floorf(ymax) + 0.5);
			//		boundary_limit(y_start, 0, height - 1);
			//		boundary_limit(y_end, 0, height - 1);

			//		for (int px_y = y_start; px_y <= y_end; px_y++) {
			//			int count = 0;
			//			float x_inter[3];
			//			for (int j = 0; j < 3; j++) {
			//				if (j == horizontal_index) continue; // 扫描线不与水平线求交
			//				ScreenCoordXYZ &from = coords[j];
			//				ScreenCoordXYZ &to = coords[(j + 1) % 3];
			//				if ((px_y - from[Y]) * (px_y - to[Y]) <= 0) { //交点存在
			//					float k = (px_y - coords[j][Y]) / vecs[j][Y];
			//					x_inter[count] = vecs[j][X] * k + coords[j][X];
			//					count++;
			//				}
			//			}

			//			float xmin = FLT_MAX, xmax = FLT_MIN;
			//			for (int j = 0; j < count; j++) {
			//				float x = x_inter[j];
			//				if (x < xmin) {
			//					xmin = x;
			//				}
			//				if (x > xmax) {
			//					xmax = x;
			//				}
			//			}

			//			if (count == 2) { // 扫到了！
			//				//if (x_inter[0] > x_inter[1]) std::swap(x_inter[0], x_inter[1]);
			//				int x_start, x_end;
			//				x_start = int(ceilf(xmin) + 0.5);
			//				x_end = int(floorf(xmax) + 0.5);

			//				boundary_limit(x_start, 0, width - 1);
			//				boundary_limit(x_end, 0, width - 1);

			//				for (int px_x = x_start; px_x <= x_end; px_x++) {
			//					ScreenCoordXY u(px_x, px_y);
			//					Vector<3> k = get_corrected_bilinear(coords, u);

			//					float depth = apply_bilinear(coords, k, [](ScreenCoordXYZ coord) {return coord[Z]; });
			//					Pixel &p = pixels[px_y * width + px_x];
			//					if (depth >= camera.depth_minimum && depth < p.depth) {
			//						p.color = apply_bilinear(vertexes, k, [](Vertex vert) {return vert.color; });
			//						p.depth = depth;
			//					}
			//				}
			//			}
			//		}
			//	}
			//}
		}


		//void Core3DContext::draw_line(Line &line) {

		//	Vector<3> start = line.transformation.apply(Vector<3>(), true);
		//	Vector<3> end = line.transformation.apply(line.orientation, true);
		//	ScreenCoordXYZ coord_start = project_to_screen(start);
		//	ScreenCoordXYZ coord_end = project_to_screen(end);

		//	ScreenCoordXYZ screen_dir = coord_end - coord_start;
		//	float len = screen_dir.norm();
		//	ScreenCoordXYZ step = screen_dir / len / 2;
		//	ScreenCoordXYZ cur = coord_start;
		//	for (int i = 0; i < len * 2; i++) {
		//		float depth = cur[2];
		//		if (depth >= camera.depth_minimum) {
		//			int x = (int)(cur[X]);
		//			int y = (int)(cur[Y]);
		//			if (x >= 0 && x < width && y >= 0 && y < height) {
		//				Pixel &p = pixels[y * width + x];
		//				if (depth < p.depth) {
		//					p.color = Color(255.0, 255.0, 255.0);
		//				}
		//			}
		//		}
		//		cur = cur + step;
		//	}
		//}

		void Core3DContext::rasterize() {
			for (auto &triangle : buffer_triangles) {
				ScreenCoordXYZ coords[3];

			
				Vertex vertexes[3];
				for (int i = 0; i < 3; i++) {
					auto &vc = buffer_vertexes[triangle.index[i]];
					coords[i] = vc.coord;
					vertexes[i] = vc.vertex;
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

					if (count == 2) { // 扫到了！
									  //if (x_inter[0] > x_inter[1]) std::swap(x_inter[0], x_inter[1]);
						int x_start, x_end;
						x_start = int(ceilf(x_inter_min) + 0.5);
						x_end = int(floorf(x_inter_max) + 0.5);

						boundary_limit(x_start, 0, width - 1);
						boundary_limit(x_end, 0, width - 1);

						for (int px_x = x_start; px_x <= x_end; px_x++) {
							ScreenCoordXY u(px_x, px_y);
							Vector<3> k = get_corrected_bilinear(u, info);

							float depth = apply_bilinear(coords, k, [](ScreenCoordXYZ coord) {return coord[Z]; });
							Pixel &p = pixels[px_y * width + px_x];
							if (depth >= camera.depth_minimum && depth < p.depth) {
								p.color = apply_bilinear(vertexes, k, [](Vertex vert) {return vert.color; });
								p.depth = depth;
							}
						}
					}
				}
			}

		}

		ScreenCoordXYZ Core3DContext::project_to_screen(Vector<3> pos) {
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

		//Vector<3> perspect_bilinear_correct(float z[3], Vector<3> bilinear)
		//{
		//	Vector<3> vec;
		//	float denom = 0.0f;
		//	for (int i = 0; i < 3; i++) {
		//		denom += bilinear[i] * z[i];
		//		vec[i] = bilinear[i] / z[i];
		//	}
		//	return vec / denom;
		//}

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


		Vector<3> get_corrected_bilinear(ScreenCoordXY u, bilinear_info &info) {
			if (info.ok) {
				Vector<2> vec = u - info.base;
				Vector<2> k12 = vec2_decompose_fast(info.basis, vec, info.det);

				Vector<3> k(1 - k12[0] - k12[1], k12[0], k12[1]);

				Vector<3> out;
				float denom = 0.0f;
				for (int i = 0; i < 3; i++) {
					denom += k[i] / info.z[i];
					out[i] = k[i] / info.z[i];
				}
				return out / denom;
			}
			else {
				return Vector<3>();
			}
		}

		Vector<2> vec2_decompose_fast(Vector<2> b[2], Vector<2> vec, float det) {
			Vector<2> ret;
			ret[0] = b[1][1] * vec[0] - b[1][0] * vec[1];
			ret[1] = -b[0][1] * vec[0] + b[0][0] * vec[1];
			return ret / det;
		}

		Vector<2> vec2_decompose(Vector<2> b[2], Vector<2> vec, bool &ok)
		{
			float det = b[0][0] * b[1][1] - b[1][0] * b[0][1];
			float sin = det / b[0].norm() / b[1].norm();
			//float sin = det * det / (b[0][0] * b[0][0] + b[0][1] * b[0][1]) / (b[1][0] * b[1][0] + b[1][1] * b[1][1]);
			if (fabs(sin) <= basis_abs_sin_threshold) {
				ok = false;
				return Vector<2>();
			}
			return vec2_decompose_fast(b, vec, det);

		}

	}
}
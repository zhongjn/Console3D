#include "core.h"
#include <cfloat>
#include <algorithm>
#include <functional>
namespace console3d {
	namespace core {
		//Vector<3> perspect_bilinear_correct(float z[3], Vector<3> bilinear);
		Vector<3> get_corrected_bilinear(ScreenCoordXYZ s[3], ScreenCoordXY u);
		Vector<2> vec2_decompose(Vector<2> b[2], Vector<2> vec);
		ScreenCoordXY xyz_to_xy(ScreenCoordXYZ s);

		//template<typename TS, typename TV>
		//TV apply_bilinear(TS sources[3], std::function<TV(TS)>& value_selector, Vector<3> coeffients) {
		//	TV accum = TV();
		//	for (int i = 0; i < 3; i++) {
		//		accum += value_selector(sources[i])* coeffients[i];
		//	}
		//	return accum;
		//}



		template<typename TS, typename F, typename TV = std::result_of<F(TS)>::type>
		TV apply_bilinear(TS sources[3], Vector<3> coeffients, F& func) {
			TV accum = TV();
			for (int i = 0; i < 3; i++) {
				accum = accum + func(sources[i]) * coeffients[i];
			}
			return accum;
		}

		Vector<2> vec2_decompose(Vector<2> basis[2], Vector<2> vec);
		Vector<2> xyz_to_xy(ScreenCoordXYZ s);

		Core3DContext::Core3DContext(short w, short h) {
			height = h;
			width = w;
			pixels = new Pixel[height * width];
		}

		void Core3DContext::draw_begin() {
			for (int i = 0; i < height*width; i++) {
				pixels[i].color = { 0, 0, 0 };
				pixels[i].depth = FLT_MAX;
			}
			camera_detransform = camera.transformation.inverse();
			combined_transformation = camera_detransform * world_transformation;
		}

		void Core3DContext::draw_end()
		{
		}


		void Core3DContext::draw_polyhedron(Polyhedron &polyhedron) {
			std::vector<Vertex> vertexes_all(polyhedron.vertexes);
			if (vertexes_all.size() >= 3) { // 至少形成一个三角形
				auto trans = polyhedron.transformation;
				std::vector<ScreenCoordXYZ> coords_all;
				coords_all.reserve(vertexes_all.size());
				for (auto& v : vertexes_all) {
					v.position = trans.apply(v.position, true);
					coords_all.emplace_back(project_to_screen(v.position));
				}
				for (int i = 2; i < vertexes_all.size(); i++) {
					ScreenCoordXYZ *coords = &coords_all[i - 2];
					Vertex *vertexes = &vertexes_all[i - 2];
					Vector<3> vecs[3] = { coords_all[1] - coords_all[0] , coords_all[2] - coords_all[1], coords_all[0] - coords_all[2] };
					int horizontal_index = -1;
					bool all_horizontal = false;
					for (int j = 0; j < 3; j++) {
						if (vecs[j][Y] == 0.0f) {
							if (horizontal_index == -1) {
								horizontal_index = j;
							}
							else {
								all_horizontal = true;
								break;
							}
						}
					}
					if (all_horizontal) continue;


					//确定三点中最大与最小的y值
					float ymin = FLT_MAX, ymax = FLT_MIN;
					for (int j = 0; j < 3; j++) {
						float y = coords[j][Y];
						if (y < ymin) {
							ymin = y;
						}
						if (y > ymax) {
							ymax = y;
						}
					}


					//
					int scan_start = int(ceilf(ymin) + 0.5); // +0.5四舍五入，防止浮点误差
					int scan_end = int(floorf(ymax) + 0.5);
					for (int px_y = scan_start; px_y <= scan_end; px_y++) {
						int count = 0;
						float x[2]; // 至多有两个交点
						for (int j = 0; j < 3; j++) {
							if (j == horizontal_index) continue; // 扫描线不与水平线求交
							ScreenCoordXYZ &from = coords[j];
							ScreenCoordXYZ &to = coords[(j + 1) % 3];
							if ((px_y - from[Y]) * (px_y - to[Y]) <= 0) { //交点存在
								float k = (px_y - coords[j][Y]) / vecs[j][Y];
								x[count] = vecs[j][X] * k + coords[j][X];
								count++;
							}
						}
						if (count == 2) { // 扫到了！
							if (x[0] > x[1]) std::swap(x[0], x[1]);
							int x_start, x_end;
							x_start = int(ceilf(x[0]) + 0.5);
							x_end = int(floorf(x[1]) + 0.5);
							for (int px_x = x_start; px_x <= x_end; px_x++) {
								ScreenCoordXY u(px_x, px_y);
								Vector<3> k = get_corrected_bilinear(coords, u);

								float depth = apply_bilinear(coords, k, [](ScreenCoordXYZ coord) {return coord[Z]; });
								Pixel &p = pixels[px_y * width + px_x];
								if (depth >= camera.depth_minimum && depth < p.depth) {
									p.color = apply_bilinear(vertexes, k, [](Vertex vert) {return vert.color; });
								}
							}
						}
					}
				}
			}
		}


		void Core3DContext::draw_line(Line &line) {

			Vector<3> start = line.transformation.apply(Vector<3>(), true);
			Vector<3> end = line.transformation.apply(line.orientation, true);
			ScreenCoordXYZ coord_start = project_to_screen(start);
			ScreenCoordXYZ coord_end = project_to_screen(end);

			ScreenCoordXYZ screen_dir = coord_end - coord_start;
			float len = screen_dir.norm();
			ScreenCoordXYZ step = screen_dir / len / 2;
			ScreenCoordXYZ cur = coord_start;
			for (int i = 0; i < len * 2; i++) {
				float depth = cur[2];
				if (depth >= camera.depth_minimum) {
					int x = (int)(cur[X]);
					int y = (int)(cur[Y]);
					if (x >= 0 && x < width && y >= 0 && y < height) {
						Pixel &p = pixels[y * width + x];
						if (depth < p.depth) {
							p.color = Color(255.0, 255.0, 255.0);
						}
					}
				}
				cur = cur + step;
			}
		}

		ScreenCoordXYZ Core3DContext::project_to_screen(Vector<3> pos) {
			// (x,y)=ScreenCoord, x=vertical, y=horizontal, TopLeft=(0,0), BottomRight=(height,width)
			// z = depth
			Vector<3> temp;
			pos = combined_transformation.apply(pos, true);
			temp[0] = pos[0] / pos[2] / camera.lens_size.width + 0.5f;
			temp[1] = pos[1] / pos[2] / camera.lens_size.height + 0.5f;

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

		Vector<3> get_corrected_bilinear(ScreenCoordXYZ s[3], ScreenCoordXY u) {
			Vector<2> vec2[3];
			for (int i = 0; i < 3; i++) {
				vec2[i] = xyz_to_xy(s[i]);
			}
			Vector<2> basis[2] = { vec2[1] - vec2[0],	vec2[2] - vec2[0] };
			Vector<2> k12 = vec2_decompose(basis, u - vec2[0]);
			Vector<3> k(1 - k12[0] - k12[1], k12[0], k12[1]);

			Vector<3> out;
			float denom = 0.0f;
			for (int i = 0; i < 3; i++) {
				denom += k[i] / s[i][Z];
				out[i] = k[i] / s[i][Z];
			}
			return out / denom;

		}
		Vector<2> vec2_decompose(Vector<2> b[2], Vector<2> vec)
		{
			float det = b[0][0] * b[1][1] - b[1][0] * b[0][1];
			Vector<2> ret;
			ret[0] = b[1][1] * vec[0] - b[1][0] * vec[1];
			ret[1] = -b[0][1] * vec[0] + b[0][0] * vec[1];
			return ret / det;
		}

	}
}
#pragma once
#include "core.h"
#include <Windows.h>
namespace console3d {
	namespace present {
		void initialize(HWND hwnd, int w, int h, int pixel_size = 5);
		void close();
		void present();
		void set_pixel(int x, int y, core::Color color);
		void set_all_pixels(core::Core3DContext &context);
	}
}
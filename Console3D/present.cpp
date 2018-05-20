#include "present.h"
#include <Windows.h>
namespace console3d {
	namespace present {
		struct tempcolor {
			BYTE b;
			BYTE g;
			BYTE r;
		};
		static int psize;
		static int owidth, oheight;
		static int width, height;
		static tempcolor* pixels;
		static HWND myhwnd;
		static HDC mydc, memdc;
		static HBITMAP hb;

		void initialize(HWND hwnd, int w, int h, int pixel_size) {
			owidth = w;
			oheight = h;
			width = w * pixel_size;
			height = h * pixel_size;
			psize = pixel_size;

			myhwnd = hwnd;
			mydc = GetDC(hwnd);
			memdc = CreateCompatibleDC(mydc);

			BITMAPINFO binfo;
			BITMAPINFOHEADER header;
			header.biSize = (UINT)sizeof(BITMAPINFO);
			header.biWidth = width;
			header.biHeight = height;
			header.biBitCount = 24;
			header.biPlanes = 1;
			header.biCompression = 0;
			binfo.bmiHeader = header;

			void* bmpdata = nullptr;
			hb = CreateDIBSection(memdc, &binfo, 0, &bmpdata, nullptr, 0);
			pixels = (tempcolor*)bmpdata;
			SelectObject(memdc, hb);
		}

		void close() {
			DeleteObject(hb);
			DeleteDC(memdc);
			ReleaseDC(myhwnd, mydc);
		}

		void present() {
			BitBlt(mydc, 0, 0, width, height, memdc, 0, 0, SRCCOPY);
		}

		void set_pixel(int x, int y, core::Color color) {
			tempcolor c;
			for (int i = 0; i < 3; i++) {
				if (color[i] < 0) {
					color[i] = 0;
				}
				else if (color[i] > 255) {
					color[i] = 255;
				}
			}
			c.r = (BYTE)color[0];
			c.g = (BYTE)color[1];
			c.b = (BYTE)color[2];
			for (int i = 0; i < psize; i++) {
				for (int j = 0; j < psize; j++) {
					pixels[((oheight - 1 - y) * psize + j) * width + (x * psize + i)] = c;
				}
			}
		}
	}
}
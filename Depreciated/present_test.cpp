#include <windows.h>
#include "core.h"
//#include "present.h"
using namespace console3d;

struct tempcolor {
	BYTE r, g, b;
};

static int width = 800, height = 600;
static tempcolor* pixels;
static HWND myhwnd;
static HDC mydc, memdc;
static HBITMAP hb;

int main2() {


	myhwnd = GetConsoleWindow();
	mydc = GetDC(myhwnd);
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

	while (true) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				pixels[j * width + i] = tempcolor{ 100, 100, 100 };
			}
		}
		BitBlt(mydc, 0, 0, width, height, memdc, 0, 0, SRCCOPY);
		Sleep(15);
	}
}
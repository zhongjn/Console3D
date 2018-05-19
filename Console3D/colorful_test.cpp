#include <windows.h>
#include <iostream>
#include <cmath>

#include <cstdlib>
#include <ctime>
#define random(a,b) (rand()%(b-a+1)+a)

//#pragma comment(lib, "Gdiplus.lib")

using namespace std;
//using namespace Gdiplus;

struct tempcolor {
	BYTE r, g, b;
};

#define PI 3.14

int main1()
{
	srand((unsigned)time(NULL));
	//Get a console handle
	HWND myconsole = GetConsoleWindow();
	//Get a handle to device context

	//ULONG_PTR m_pGdiplusToken;
	//GdiplusStartupInput gdiplusStartupInput;
	//GdiplusStartup(&m_pGdiplusToken, &gdiplusStartupInput, NULL);

	int i2 = 0;
	int ii = 0;
	HDC mydc = GetDC(myconsole);
	auto memdc = CreateCompatibleDC(mydc);

	HBITMAP hb;

	BITMAPINFO binfo;
	BITMAPINFOHEADER header;
	header.biSize = (UINT)sizeof(BITMAPINFO);
	header.biWidth = 800;
	header.biHeight = 600;
	header.biBitCount = 24;
	header.biPlanes = 1;
	header.biCompression = 0;
	binfo.bmiHeader = header;

	void* bmpdata = nullptr;
	hb = CreateDIBSection(memdc, &binfo, 0, &bmpdata, nullptr, 0);
	SelectObject(memdc, hb);
	
	while (true) {
		i2++;
		ii += 1;
		//ii %= 255;
		

		


		tempcolor *ptr =(tempcolor *) bmpdata;
		for (int i = 0; i < 800 * 600; i++) {
			int cc = (i * 5 + ii) % 255;
			tempcolor &c = ptr[i];
			c.r = cc;
			c.g = cc;
			c.b = cc;

		}

		

		BitBlt(mydc, 0, 0, 800, 600, memdc, 0, 0, SRCCOPY);

		
		

		
	}
	
	ReleaseDC(myconsole, mydc);
	DeleteObject(hb);
	DeleteDC(memdc);

	cin.ignore();
	return 0;
}
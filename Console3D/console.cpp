#include "console.h"

namespace console3d {

	namespace console {
		static HANDLE handle_console;

		static CHAR_INFO* disp;

		static short height_ch;
		static short width_ch;
		static short height_actual;
		static short width_actual;

		static COORD size;
		static COORD src;
		static SMALL_RECT dest;

		void initialize(short height, short width) {
			handle_console = GetStdHandle(STD_OUTPUT_HANDLE);

			height_ch = height;
			width_ch = width;

			height_actual = height;
			width_actual = width * 2;

			size = { width_actual, height_actual };
			src = { 0, 0 };
			dest = { 0, 0, width_actual, height_actual };

			disp = new CHAR_INFO[height_actual * width_actual];
			for (int i = 0; i < height_actual * width_actual; i++) {
				disp[i].Attributes = 0x0007;
				disp[i].Char.AsciiChar = ' ';
			}
		}

		void set_char(short i, short j, char c) {
			if (i >= 0 && i < height_ch && j >= 0 && j < width_ch) {
				disp[i * width_actual + j * 2].Char.AsciiChar = c;
			}
		}

		void render()
		{
			COORD pos = { 0,0 };
			SetConsoleCursorPosition(handle_console, pos);
			WriteConsoleOutput(handle_console, (CHAR_INFO *)disp, size, src, &dest);
		}

	}
}